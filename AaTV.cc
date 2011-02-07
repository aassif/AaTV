#include <QWizard>
#include <QPainter>
#include <QPaintEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QTextEdit>
//#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTreeWidget>
//#include <QHeaderView>
#include <QDir>
#include <fstream> // debug
#include <sstream>
#include "qfilewidget.h"
#include "AaTVDB.h"

class QNetworkPixmap : public QFrame
{
  Q_OBJECT

  protected:
    QSize m_size;
    QPixmap m_pixmap;
    QNetworkAccessManager m_network;
    QNetworkReply * m_reply;

  private:
    void init ()
    {
      QSizePolicy policy (QSizePolicy::Preferred, QSizePolicy::Preferred);
      policy.setHeightForWidth (true);
      policy.setHorizontalStretch (1);
      policy.setVerticalStretch (0);
      setSizePolicy (policy);
      clear ();
    }

  public:
    QNetworkPixmap (QWidget * parent = NULL) :
      QFrame (parent),
      m_size (0, 0),
      m_pixmap (),
      m_reply (NULL)
    {
      init ();
    }

    QNetworkPixmap (int w, int h, QWidget * parent = NULL) :
      QFrame (parent),
      m_size (w, h),
      m_pixmap (),
      m_reply (NULL)
    {
      init ();
    }

    virtual QSize sizeHint () const
    {
      return m_pixmap.size ();
    }

    virtual int heightForWidth (int width) const
    {
      int w = m_pixmap.width ();
      int h = m_pixmap.height ();
      return w != 0 ? (width * h) / w : 0;
    }

  public slots:
    void query (const QString & url)
    {
      if (m_reply != NULL)
      {
        m_reply->disconnect (SIGNAL(finished()), this, 0);
        m_reply->close ();
        m_reply = NULL; // FIXME: cannot delete!
      }

      m_reply = m_network.get (QNetworkRequest (url));
      connect (m_reply, SIGNAL(finished()), SLOT(setPixmap()));
      clear ();
    }

    void clear ()
    {
      m_pixmap = QPixmap (m_size);
      m_pixmap.fill (palette ().color (QPalette::Dark));
      updateGeometry ();
      update ();
    }

  private slots:
    void setPixmap ()
    {
      QImage image = QImage::fromData (m_reply->readAll ());
      m_pixmap = QPixmap::fromImage (image);
      m_reply->close ();
      m_reply = NULL; // FIXME: delete!
      updateGeometry ();
      update ();
    }

  protected:
    virtual void paintEvent (QPaintEvent * e)
    {
      QPainter p (this);
      p.drawPixmap (0, 0, m_pixmap.scaled (size (), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
};

namespace AaTV
{
// AaTV::DataQuery /////////////////////////////////////////////////////////////

  class DataQuery : public QThread
  {
    Q_OBJECT

    protected:
      Aa::TVDB::Data m_data;
      QNetworkAccessManager m_network;
      QNetworkReply * m_reply;
      mutable QMutex m_mutex;

    public:
      DataQuery (QObject * parent = NULL);
      const Aa::TVDB::Data    & data    ()      const {return m_data;}
      const Aa::TVDB::Series  & series  (int k) const {return m_data.series [k];}
      const Aa::TVDB::Episode & episode (int k) const {return m_data.episodes [k];}

    protected:
      virtual void run ();

    public slots:
      void query (const QUrl &);
  };

// AaTV::PageSeries ////////////////////////////////////////////////////////////

  class PageSeries : public QWizardPage
  {
    Q_OBJECT

    protected:
      DataQuery m_tvdb;
      QLineEdit m_title;
      QComboBox m_combo;
      QTextEdit m_overview;

    public:
      // Constructor.
      PageSeries (QWidget * parent = NULL);
      const Aa::TVDB::Series * currentItem () const;
      // QWizardPage.
      virtual bool isComplete () const;
      virtual int nextId () const;

    private slots:
      void populate ();
      void update (int);

    public slots:
      void query (const QString & title);
  };

// AaTV::PageEpisodes //////////////////////////////////////////////////////////

  class PageEpisodes : public QWizardPage
  {
    Q_OBJECT

    protected:
      PageSeries * m_series;
      DataQuery m_tvdb;
      QNetworkPixmap m_banner;
      QTreeWidget m_tree;
      QExistingDirectoryWidget m_directory;

    private:
      static Qt::CheckState RecursiveCheckState (QTreeWidgetItem *,
                                                 int column,
                                                 Qt::CheckState);

    public:
      // Constructor.
      PageEpisodes (PageSeries *, QWidget * parent = NULL);
      QStringList checkedItems () const;
      QString directory () const;
      // QWizardPage.
      virtual void initializePage ();
      virtual bool isComplete () const;
      virtual int nextId () const;

    private slots:
      void populate ();
      void update (QTreeWidgetItem *, int column);

    public slots:
      void query (const Aa::TVDB::Series *);
  };

// AaTV::Wizard ////////////////////////////////////////////////////////////////

  class Wizard : public QWizard
  {
    public:
      enum
      {
        Intro = 0,
        Series = 1,
        Episodes = 2,
        Final = 3
      };

    private:
      class PageFinal : public QWizardPage
      {
        private:
          PageEpisodes * m_episodes;
          QProgressBar m_progress;
          QTextEdit m_blabla;
          QThread * m_thread;

        public:
          PageFinal (PageEpisodes *);
          virtual void initializePage ();
          virtual int nextId () const {return -1;}
      };

    public:
      Wizard (QWidget * parent = 0);
  };

////////////////////////////////////////////////////////////////////////////////
// AaTV::DataQuery /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  DataQuery::DataQuery (QObject * parent) :
    QThread (parent),
    m_data (),
    m_network (),
    m_reply (NULL),
    m_mutex ()
  {
  }

  void DataQuery::query (const QUrl & url)
  {
    if (m_reply != NULL)
    {
      m_reply->disconnect (SIGNAL(finished()), this, 0);
      m_reply->close ();
      m_reply = NULL;
    }

    m_reply = m_network.get (QNetworkRequest (url));
    connect (m_reply, SIGNAL(finished()), SLOT(start()));
  }

  void DataQuery::run ()
  {
    m_mutex.lock ();
    QByteArray bytes = m_reply->readAll ();
    m_mutex.unlock ();

    try
    {
      std::istringstream iss (bytes.data ());
      Aa::TVDB::Data data;
      Aa::TVDB::DataXml::Parse (&data, iss);

      m_mutex.lock ();
      m_reply->close ();
      m_reply = NULL;
      m_data = data;
      m_mutex.unlock ();
    }
    catch (Aa::ParseError & e)
    {
      //emit error (e.what ());
    }
  }

////////////////////////////////////////////////////////////////////////////////
// AaTV::PageSeries ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  PageSeries::PageSeries (QWidget * parent) :
    QWizardPage (parent),
    m_tvdb (this),
    m_title (this),
    m_combo (this),
    m_overview (this)
  {
    QString title =
      tr("PageSettings");

    QString subtitle =
      tr("Bla bla bla.");

    setTitle (title);
    setSubTitle (subtitle);
    //setCommitPage (true);

    m_overview.setReadOnly (true);

    QVBoxLayout * layout = new QVBoxLayout;
    //layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (2);
    layout->addWidget (new QLabel (tr("Titre de la série")));
    layout->addWidget (&m_title);
    layout->addSpacing (6);
    layout->addWidget (new QLabel (tr("Suggestions")));
    layout->addWidget (&m_combo);
    layout->addSpacing (6);
    layout->addWidget (new QLabel (tr("Résumé")));
    layout->addWidget (&m_overview);
    setLayout (layout);

    connect (&m_title, SIGNAL(textChanged(const QString &)), this, SLOT(query(const QString &)));
    connect (&m_tvdb,  SIGNAL(finished()),                   this, SLOT(populate()));
    connect (&m_combo, SIGNAL(currentIndexChanged(int)),     this, SLOT(update(int)));
  }

  const Aa::TVDB::Series * PageSeries::currentItem () const
  {
    int k = m_combo.currentIndex ();
    return k != -1 ? &(m_tvdb.series (k)) : NULL;
  }

  int PageSeries::nextId () const
  {
    return Wizard::Episodes;
  }

  bool PageSeries::isComplete () const
  {
    return m_combo.currentIndex () != -1;
  }

  void PageSeries::query (const QString & title)
  {
    static const QString BASE =
      "http://www.thetvdb.com/api/GetSeries.php?seriesname=";

    m_combo.clear ();
    m_overview.clear ();
    emit completeChanged ();

    if (! title.isEmpty ())
      m_tvdb.query (QUrl (BASE + title));
  }

  QString qtvdb_utf8 (const std::string & utf8)
  {
    return QString::fromUtf8 (utf8.c_str ()).trimmed ();
  }

  void PageSeries::populate ()
  {
    const Aa::TVDB::Data & data = m_tvdb.data ();
    for (std::vector<Aa::TVDB::Series>::const_iterator
           i = data.series.begin (), e = data.series.end (); i != e;)
    {
      const Aa::TVDB::Series & series = *(i++);
      QString id   = qtvdb_utf8 (series.id);
      QString name = qtvdb_utf8 (series.name);
      m_combo.addItem (name, id);
    }

    emit completeChanged ();
  }

  void PageSeries::update (int k)
  {
    if (k != -1)
    {
      const Aa::TVDB::Series & series = m_tvdb.series (k);
      QString overview = qtvdb_utf8 (series.overview);
      m_overview.setText (overview);
    }
    else
      m_overview.clear ();
  }

////////////////////////////////////////////////////////////////////////////////
// AaTVEpisodes ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  PageEpisodes::PageEpisodes (PageSeries * series, QWidget * parent) :
    QWizardPage (parent),
    m_series (series),
    m_tvdb (this),
    m_banner (758, 140, this),
    m_tree (this),
    m_directory (this)
  {
    QString title =
      tr("PageSettings");

    QString subtitle =
      tr("Bla bla bla.");

    setTitle (title);
    setSubTitle (subtitle);
    setCommitPage (true);

    m_tree.setColumnCount (1);
    //m_tree.setEditTriggers (QAbstractItemView::NoEditTriggers);
    m_tree.setEditTriggers (QAbstractItemView::DoubleClicked);
    m_tree.setSelectionBehavior (QAbstractItemView::SelectRows);
    m_tree.setWordWrap (false);
    m_tree.setAlternatingRowColors (true);

    QTreeWidgetItem * header = m_tree.headerItem ();
    header->setHidden (true);

    QVBoxLayout * layout = new QVBoxLayout;
    //layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (2);
    layout->addWidget (&m_banner, 0, Qt::AlignHCenter);
    layout->addSpacing (6);
    //layout->addWidget (new QLabel (tr("Épisodes")));
    layout->addWidget (&m_tree);
    layout->addSpacing (6);
    layout->addWidget (new QLabel (tr("Destination")));
    layout->addWidget (&m_directory);
    setLayout (layout);

    connect (&m_tvdb, SIGNAL(finished()), this, SLOT(populate()));
    connect (&m_tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
             this,    SLOT(update(QTreeWidgetItem *, int)));
  }

  void PageEpisodes::initializePage ()
  {
    this->query (m_series->currentItem ());
  }

  int PageEpisodes::nextId () const
  {
    return Wizard::Final;
  }

  bool PageEpisodes::isComplete () const
  {
    QStringList items = this->checkedItems ();
    return ! items.isEmpty ();
  }

  void PageEpisodes::query (const Aa::TVDB::Series * series)
  {
    m_banner.clear ();
    m_tree.clear ();

    if (series != NULL)
    {
      static const QString BANNERS = "http://www.thetvdb.com/banners/";
      m_banner.query (BANNERS + qtvdb_utf8 (series->banner));

      static const QString API = "http://www.thetvdb.com/api/";
      m_tvdb.query (QUrl (API + Aa::TVDB::API_KEY.c_str () + "/series/" + qtvdb_utf8 (series->id) + "/all/"));
    }
  }

  QString scene_capitalize_aux (const QString & text)
  {
    return text.mid (0, 1).toUpper () + text.mid (1);
  }

  QString scene_capitalize (const QString & text)
  {
    QStringList list0 =
      text.split (QRegExp ("\\W+"), QString::SkipEmptyParts);

    QStringList list1;
    for (int i = 0; i < list0.size (); ++i)
      list1 << scene_capitalize_aux (list0 [i]);

    return list1.join (".");
  }

  QString scene_episode (const QString & season, const QString & episode)
  {
    return QString ("S%1E%2").
             arg (season,  2, '0').
             arg (episode, 2, '0');
  }

  void PageEpisodes::populate ()
  {
    QString show = qtvdb_utf8 (m_tvdb.series (0).name);

    std::map<QString, QTreeWidgetItem *> seasons;

    const Aa::TVDB::Data & data = m_tvdb.data ();
    for (std::vector<Aa::TVDB::Episode>::const_iterator
           i = data.episodes.begin (), e = data.episodes.end (); i != e;)
    {
      const Aa::TVDB::Episode & episode = *(i++);
      QString id     = qtvdb_utf8 (episode.id);
      QString season = qtvdb_utf8 (episode.season);
      QString number = qtvdb_utf8 (episode.number);
      QString name   = qtvdb_utf8 (episode.name);

      std::map<QString, QTreeWidgetItem *>::iterator
        f = seasons.find (season);
      if (f == seasons.end ())
      {
        QString text = QString ("S%1").arg (season, 2, '0');
        QTreeWidgetItem * item = new QTreeWidgetItem (QStringList (text));
        item->setCheckState (0, Qt::Unchecked);
        m_tree.addTopLevelItem (item);
        f = seasons.insert (std::make_pair (season, item)).first;
      }

      QString code = QString ("%1x%2").arg (season).arg (number, 2, '0');
      QString text = code + (name.isEmpty () ? QString () : (" - " + name));

      /*QString text = scene_capitalize (show) + '.'
                   + scene_episode (season, number) + '.'
                   + scene_capitalize (name) + '.'
                   + "DVDRip" + '.' + "XviD" + '-' + "AASSiF";*/

      QTreeWidgetItem * item = new QTreeWidgetItem (QStringList (text));
      item->setFlags (Qt::ItemIsSelectable
                    | Qt::ItemIsUserCheckable
                    | Qt::ItemIsEnabled
                    | Qt::ItemIsEditable);
      item->setCheckState (0, Qt::Unchecked);
      f->second->addChild (item);
    }
  }

  Qt::CheckState
    PageEpisodes::RecursiveCheckState (QTreeWidgetItem * parent,
                                       int column,
                                       Qt::CheckState check)
  {
    int n = 0;
    for (int i = 0; i < parent->childCount (); ++i)
    {
      QTreeWidgetItem * child = parent->child (i);
      if (child->checkState (column) != check) ++n;
    }
    return (n == 0 ? check : Qt::PartiallyChecked);
  }

  void PageEpisodes::update (QTreeWidgetItem * item, int column)
  {
    // Block signals.
    m_tree.blockSignals (true);

    Qt::CheckState check = item->checkState (column);

    QTreeWidgetItem * parent = item->parent ();
    if (parent != NULL)
    {
      Qt::CheckState c = RecursiveCheckState (parent, column, check);
      parent->setCheckState (column, c);
    }

    for (int i = 0; i < item->childCount (); ++i)
    {
      QTreeWidgetItem * child = item->child (i);
      child->setCheckState (column, check);
    }

    // Restore signals.
    m_tree.blockSignals (false);

    emit completeChanged ();
  }

  QStringList PageEpisodes::checkedItems () const
  {
    QStringList strList;
    for (int i = 0; i < m_tree.topLevelItemCount (); ++i)
    {
      QTreeWidgetItem * season = m_tree.topLevelItem (i);
      for (int j = 0; j < season->childCount (); ++j)
      {
        QTreeWidgetItem * episode = season->child (j);
        if (episode->checkState (0) != Qt::Unchecked)
          strList << (season->text (0) + '/' + episode->text (0));
      }
    }
    return strList;
  }
  QString PageEpisodes::directory () const
  {
    return m_directory.text ();
  }

////////////////////////////////////////////////////////////////////////////////
// AaTV::Wizard::PageFinal /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  Wizard::PageFinal::PageFinal (PageEpisodes * episodes) :
    m_episodes (episodes),
    m_progress (this),
    m_blabla (this),
    m_thread (NULL)
  {
    QString title =
      tr("PageFinal");

    QString subtitle =
      tr("Bla bla bla.");

    setTitle (title);
    setSubTitle (subtitle);

    m_blabla.setReadOnly (true);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setSpacing (2);
    layout->addWidget (&m_progress);
    layout->addSpacing (6);
    layout->addWidget (&m_blabla, 1);
    setLayout (layout);
  }

  void Wizard::PageFinal::initializePage ()
  {
    static const QString RED   = "<span style=\"color:red;\">%1</span>";
    static const QString GREEN = "<span style=\"color:green;\">%1</span>";

    QDir dir (m_episodes->directory ());
    QStringList items = m_episodes->checkedItems ();

    for (int i = 0; i < items.size (); ++i)
    {
      bool okay = dir.mkpath (items [i]);
      m_blabla.append ((okay ? GREEN : RED).arg (items [i]));
    }
  }

////////////////////////////////////////////////////////////////////////////////
// AaTV::Wizard ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  AaTV::Wizard::Wizard (QWidget * parent) :
    QWizard (parent)
  {
    //PageIntro    * intro    = new PageIntro;
    PageSeries   * series   = new PageSeries;
    PageEpisodes * episodes = new PageEpisodes (series);
    PageFinal    * final    = new PageFinal    (episodes);

    //setPage (Intro,    intro);
    setPage (Series,   series);
    setPage (Episodes, episodes);
    setPage (Final,    final);
  }

} // namespace AaTV

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char ** argv)
{
  QApplication app (argc, argv);
  AaTV::Wizard wizard;
  wizard.setWindowIcon (QIcon ("icons/old-tv.16x16.png"));
  wizard.show ();
  return app.exec ();
}

////////////////////////////////////////////////////////////////////////////////

#include "AaTV.moc"
