#ifndef BEEFISH__WEBDB__STREAM_FROM_DB
#define BEEFISH__WEBDB__STREAM_FROM_DB

#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"

namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   inline size_t streamFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   )
   {

      if (path.isDeadEnd()) {
         throw std::runtime_error("Path is dead end");
      }

      size_t pageIndex  = 0;
      size_t min = path.min();
      size_t max = path.max();
      size_t byteCount = 0;
std::cerr << "StreamFromDB::min " << min << ", max " << max << endl;

      for ( pageIndex = min;
            pageIndex <= max;
            ++pageIndex )
      {

         std::string data;

         Path page = path[pageIndex];

         if (page.hasData()) {
            page.getData(data); 
            output.write(
               data.data(),
               data.size()
            );
            byteCount += data.size();
         }
         else {
            throw std::runtime_error("Page has no data");
         }
      }

      return byteCount;


   }
/*
   class StreamFromDB :
      public Path<Database::Encoding>
   {
   protected:
      ostream& _output;
      char* _page = nullptr;
      int   _position {0};
      int   _pageCount {0};
   public:

      StreamFromDB(
         const Path<Database::Encoding>& path,
         ostream& output
      ) :
         Path(path),
         _output(output)
      {
         allocatePage();
      }

      StreamFromDB(
         const StreamFromDB& source
      ) :
         Path(source),
         _output(source._output)
      {
         allocatePage();
      }

      virtual ~StreamFromDB()
      {
         delete[] _page;
      }

   protected:
      void allocatePage()
      {
         _page = new char[pageSize];
      }

      const size_t size() const {
         return
            _position +
            _pageCount * pageSize;
      }

   };
*/
}

#endif
