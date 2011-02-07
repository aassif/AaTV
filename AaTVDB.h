#ifndef __AA_TVDB__
#define __AA_TVDB__

#include <vector>
#include <AaXml>

namespace Aa
{
  namespace TVDB
  {
    // TheTVDB.com API key.
    static const XmlString API_KEY = "49043ECD1409D413";

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::Series ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class Series
    {
      public:
        XmlString id;
        XmlString name;
        XmlString overview;
        XmlString banner;
    };

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::Episode ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class Episode
    {
      public:
        XmlString id;
        XmlString season;
        XmlString number;
        XmlString name;
        XmlString overview;
        //XmlString first_aired;
    };

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::Data //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class Data
    {
      public:
        std::vector<Series>  series;
        std::vector<Episode> episodes;

      public:
        void clear ();
    };

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::SeriesXml /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class SeriesIdXml : public XmlParserImpl<Data>
    {
      public:
        SeriesIdXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class SeriesNameXml : public XmlParserImpl<Data>
    {
      public:
        SeriesNameXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class SeriesOverviewXml : public XmlParserImpl<Data>
    {
      public:
        SeriesOverviewXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class SeriesBannerXml : public XmlParserImpl<Data>
    {
      public:
        SeriesBannerXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class SeriesXml : public XmlParserImpl<Data>
    {
      public:
        SeriesXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::EpisodeXml ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class EpisodeIdXml : public XmlParserImpl<Data>
    {
      public:
        EpisodeIdXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class EpisodeSeasonXml : public XmlParserImpl<Data>
    {
      public:
        EpisodeSeasonXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class EpisodeNumberXml : public XmlParserImpl<Data>
    {
      public:
        EpisodeNumberXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class EpisodeNameXml : public XmlParserImpl<Data>
    {
      public:
        EpisodeNameXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

    class EpisodeXml : public XmlParserImpl<Data>
    {
      public:
        EpisodeXml (Data *, XmlParser * = NULL);
        virtual void parse (std::istream &) throw (ParseError);
    };

////////////////////////////////////////////////////////////////////////////////
// Aa::TVDB::DataXml ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    class DataXml : public XmlParser
    {
      public:
        DataXml (Data *, XmlParser * = NULL);

      public:
        static void Parse (Data *, std::istream &) throw (ParseError);
    };
  } // namespace TVDB
} // namespace Aa

#endif // __AA_TVDB__

