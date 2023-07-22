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
      size_t  _pageIndex {0};
      size_t  _size {0};
      size_t  _contentLength {0};
   public:

      StreamToDB(
         const Parser& parser,
         const Path<Database::Encoding> path,
         const size_t contentLength
      ) :
         Capture(parser),
         _path(path),
         _contentLength(contentLength)
      {
         _path.setData(contentLength);
         allocatePage();
      }

      StreamToDB(
         const StreamToDB& source
      ) :
         Capture(source),
         _path(source._path),
         _contentLength(source._contentLength)
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
         if (++_position >= _path.pageSize() ||
             (_contentLength > 0 &&
              _size >= _contentLength ))
         {
            flush();
         }
      }

      virtual bool flush()
      override
      {
         if (!Capture::flush())
            return false;

         if (_position > 0) {
            const std::string
               data(_page, _position);
            _path[_pageIndex++]
               .setData(data);
            
            _position = 0;
         }

         return true;
      }

      virtual void success() override
      {
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
