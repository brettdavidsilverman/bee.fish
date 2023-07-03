#ifndef BEEFISH__WEB__STREAM_TO_DB
#define BEEFISH__WEB__STREAM_TO_DB

#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"


namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   class StreamToDB :
      public Capture
   {
   protected:
      Path<Database::Encoding> _path;
      char*   _page = nullptr;
      size_t  _position {0};
      size_t  _pageCount {0};
      size_t  _size {0};

   public:

      StreamToDB(
         const Parser& parser,
         const Path<Database::Encoding> path
      ) :
         Capture(parser),
         _path(path)
      {
         
         allocatePage();
      }

      StreamToDB(
         const StreamToDB& source
      ) :
         Capture(source),
         _path(source._path)
      {
         allocatePage();
      }

      virtual ~StreamToDB()
      {
         delete[] _page;
      }

      void allocatePage()
      {
         _page = new char[_path.pageSize()];
      }

      virtual void capture(char c)
      override
      {
         _page[_position] = c;
         ++_size;
         if (++_position >= _path.pageSize())
         {
            flush();
         }
      }

      virtual void flush()
      {
         if (_position > 0) {
            const std::string
               data(_page, _position);

            _path[_pageCount++]
               .setData(data);
cerr << "StreamToDB: pageCount:" << _pageCount << endl;
           _position = 0;
         }
      }

      virtual void success() override
      {
         _path["size"] = _size;
         flush();

      }

      virtual Parser* copy()
      const override
      {
         return new StreamToDB(*this);
      }

   };

}

#endif
