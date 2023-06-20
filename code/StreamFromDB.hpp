#ifndef BEEFISH__WEB__STREAM_FROM_DB
#define BEEFISH__WEB__STREAM_FROM_DB

#include "Parser/Parser.hpp"
#include "Database/Database.hpp"


namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   ostream& operator <<
   (
      ostream& output,
      const Path<Database::Encoding>& path
   )
   {

      size_t position {0};
      size_t pageCount {0};
      size_t min = path.min();
      size_t max = path.max();


      for ( pageCount = min;
            pageCount < max;
            ++pageCount )
      {
         Data data =
            path.getDataPage(pageCount);
         output << page;
      }
   }

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

}

#endif
