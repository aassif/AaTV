#include "AaTVDB.h"

// Aa::TVDB::API_KEY ///////////////////////////////////////////////////////////

//const Aa::XmlString Aa::TVDB::API_KEY = "49043ECD1409D413";

namespace Aa
{
  namespace TVDB
  {
// Aa::TVDB::Series ////////////////////////////////////////////////////////////

// Aa::TVDB::Episode ///////////////////////////////////////////////////////////

// Aa::TVDB::Data //////////////////////////////////////////////////////////////

    void Data::clear ()
    {
      series.clear ();
      episodes.clear ();
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesIdXml ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    SeriesIdXml::SeriesIdXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("id", data, parent)
    {
    }

    void SeriesIdXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->series.back ().id = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesNameXml /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    SeriesNameXml::SeriesNameXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("SeriesName", data, parent)
    {
    }

    void SeriesNameXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->series.back ().name = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesOverviewXml /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    SeriesOverviewXml::SeriesOverviewXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("Overview", data, parent)
    {
    }

    void SeriesOverviewXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->series.back ().overview = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesBannerXml ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    SeriesBannerXml::SeriesBannerXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("banner", data, parent)
    {
    }

    void SeriesBannerXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->series.back ().banner = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesXml /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    SeriesXml::SeriesXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("Series", data, parent)
    {
      new SeriesIdXml       (data, this);
      new SeriesNameXml     (data, this);
      new SeriesOverviewXml (data, this);
      new SeriesBannerXml   (data, this);
    }

    void SeriesXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->series.push_back (Series ());
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeIdXml //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    EpisodeIdXml::EpisodeIdXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("id", data, parent)
    {
    }

    void EpisodeIdXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->episodes.back ().id = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeSeasonXml //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    EpisodeSeasonXml::EpisodeSeasonXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("SeasonNumber", data, parent)
    {
    }

    void EpisodeSeasonXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->episodes.back ().season = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeNumberXml //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    EpisodeNumberXml::EpisodeNumberXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("EpisodeNumber", data, parent)
    {
    }

    void EpisodeNumberXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->episodes.back ().number = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeNameXml ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    EpisodeNameXml::EpisodeNameXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("EpisodeName", data, parent)
    {
    }

    void EpisodeNameXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->episodes.back ().name = XmlParser::ParseText (is);
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeXml ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    EpisodeXml::EpisodeXml (Data * data, XmlParser * parent) :
      XmlParserImpl<Data> ("Episode", data, parent)
    {
      new EpisodeIdXml     (data, this);
      new EpisodeSeasonXml (data, this);
      new EpisodeNumberXml (data, this);
      new EpisodeNameXml   (data, this);
    }

    void EpisodeXml::parse (std::istream & is) throw (ParseError)
    {
      m_data->episodes.push_back (Episode ());
      XmlParser::parse (is);
    }

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesListXml /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    DataXml::DataXml (Data * data, XmlParser * parent) :
      XmlParser ("Data", parent)
    {
      new SeriesXml  (data, this);
      new EpisodeXml (data, this);
    }

    void DataXml::Parse (Data * data, std::istream & is)
      throw (ParseError)
    {
      DataXml parser (data);
      XmlParser::ParseRoot (is, &parser);
    }
  } // namespace TVDB
} // namespace Aa

