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

   inline size_t streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   );

   inline size_t streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   );

   inline bool writeHeaders(
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path,
      std::string contentType
   );

   inline size_t streamFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path,
      bool original
   )
   {
      if (original) {

         if (!path.contains("document"))
            return 0;

         return streamDocumentFromDB(
            output, path
         );
      }
      else if (path.contains("JSON"))
         return streamJSONFromDB(
            output, path["JSON"]
         );

      return 0;

   }

   inline size_t streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   )
   {
      return 0;
   }

   inline size_t streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   )
   {

      if (path.isDeadEnd() ||
         !path.hasData())
      {
         throw std::runtime_error("Path is dead end");
      }

      string contentType;
      path.getData(contentType);

      Path document = path["document"];

      if (!writeHeaders(
            output,
            document,
            contentType))
         return 0;

      // Output the document content
      size_t pageIndex  = 0;
      size_t min = document.min();
      size_t max = document.max();
      size_t byteCount = 0;

      for ( pageIndex = min;
            pageIndex <= max;
            ++pageIndex )
      {

         std::string data;

         BeeFishWeb::Path page =
            document[pageIndex];

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

   bool writeHeaders(
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> document,
      std::string contentType
   )
   {
      
      stringstream stream;

      size_t size = 0;

      if (document.hasData())
         size = (size_t)document;
        
      stream <<
         "HTTP/2.0 200 OK" << "\r\n" <<
         "Connection: keep-alive\r\n" <<
         "Content-Type: " <<
            contentType << "\r\n";
         if (size > 0)
            stream <<
               "Content-Length: " <<
               size << "\r\n";
      stream << "\r\n";

      std::string headers =
         stream.str();

      // Send the headers
      output.write(
         headers.data(),
         headers.size()
      );

      return true;
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
