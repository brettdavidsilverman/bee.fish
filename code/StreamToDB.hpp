#ifndef BEEFISH__WEB__STREAM_TO_DB
#define BEEFISH__WEB__STREAM_TO_DB

#include "Parser/Parser.hpp"
#include "Database/Database.hpp"


namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   class StreamToDB :
      public Capture,
      public Path<Database::Encoding>
   {
   protected:
      char*   _page = nullptr;
      size_t  _position {0};
      size_t  _pageCount {0};
   public:

      StreamToDB(
         const Parser& parser,
         const Path<Database::Encoding>& path
      ) :
         Capture(parser),
         Path(path)
      {
         allocatePage();
      }

      StreamToDB(
         const StreamToDB& source
      ) :
         Capture(source),
         Path(source)
      {
         allocatePage();
      }

      virtual ~StreamToDB()
      {
         delete[] _page;
      }

   protected:
      void allocatePage()
      {
         _page = new char[pageSize];
      }

      virtual void capture(char c)
      override
      {
         _page[_position] = c;
         if (++_position >= pageSize)
         {
            flush();
         }
      }

      virtual void flush()
      {
         (*this)
            [_pageCount++]
            .setDataPage(_page, pageSize);
         _position = 0;
      }

      virtual void success() override
      {
         if (_position > 0)
            flush();

      }

      const size_t size() const {
         return
            _position +
            _pageCount * pageSize;
      }

      virtual Parser* copy()
      const override
      {
         return new StreamToDB(*this);
      }

   };

}

#endif
