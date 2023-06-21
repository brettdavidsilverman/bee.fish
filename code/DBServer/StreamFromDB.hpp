#ifndef BEEFISH__WEBDB__STREAM_FROM_DB
#define BEEFISH__WEBDB__STREAM_FROM_DB

#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"


namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   inline void streamFromDB (
      ostream& output,
      Path<Database::Encoding> path
   )
   {

      if (path.isDeadEnd()) {
         throw std::runtime_error("Path is dead end");
      }

      size_t pageCount {0};
      size_t min = path.min();
      size_t max = path.max();

      for ( pageCount = min;
            pageCount <= max;
            ++pageCount )
      {
         std::string data;

         Path page = path[pageCount];

         if (page.hasData()) {
            page.getData(data); 
            output.write(
               data.data(),
               data.size()
            );
         }
         else {
            throw std::runtime_error("Page has no data");
         }
      }


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
