#ifndef BEEFISH__WEBDB__STREAM_FROM_DB
#define BEEFISH__WEBDB__STREAM_FROM_DB

#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "JSONParser.hpp"

namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;
   using namespace BeeFishWebDB;

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
      std::string contentType,
      Size contentLength
   );

   inline size_t streamFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path,
      bool original
   )
   {
      if (path.isDeadEnd() ||
            !path.hasData())
      {
         throw std::runtime_error("Path is dead end");
      }

      string contentType;
      path.getData(contentType);

      if (path.contains("JSON"))
      {
         if (!writeHeaders(
               output,
               contentType,
               0))
         {
            return 0;
         }

         return streamJSONFromDB(
            output, path["JSON"]
         );
      }

      if (path.contains("document"))
      {
         Size contentLength = (Size)
            path["document"];

         if (!writeHeaders(
            output,
            contentType,
            contentLength))
         {
            return 0;
         }

         return streamDocumentFromDB(
            output, path["document"]
         );
      }

      return 0;
   }

   inline size_t streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> jsonPath
   )
   {
//#warning here
              
      if (!jsonPath.hasData())
         throw runtime_error("JSON path has no data");

      Size size = 0;

      JSONValue* value = (JSONValue*)
         jsonPath.getData().data();

      switch (value->_type)
      {
         case JSONType::INT64:
         {
            stringstream stream;
            stream << *(int64_t*)(value->_data);
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::UINT64:
         {
            stringstream stream;
            stream << *(uint64_t*)(value->_data);
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::DOUBLE:
         {
            stringstream stream;
            
            stream << *(double*)(value->_data);
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::STRING:
         {
            std::string str((char*)value->_data, value->_size);
            str = "\"" + escape(str) + "\"";
            size += output.write(str);
            break;
         }
         case JSONType::BOOL:
         {
            string str = (*(bool*)(value->_data)
                ? "true" : "false");
            size += output.write(str);
            break;
         }
         case JSONType::_NULL:
         {
            string str = "null";
            size += output.write(str);
            break;
         }
         case JSONType::OBJECT:
         {
            size += output.write("{}");
            break;
         }
         case JSONType::ARRAY:
         {
            size += output.write("[");

            if (!jsonPath.isDeadEnd())
            {

               Size min = jsonPath.min();
               Size max = jsonPath.max();

               for (Size i = min;
                    i <= max;
                    ++i)
               {
                  size += streamJSONFromDB (
                     output,
                     jsonPath[i]
                  );

                  if (i < max)
                     size += output.write(", ");
   
               }
            }

            size += output.write("]");

            break;
         }
         default:
            throw runtime_error("Invalid type");
      }

      return size;
      
   }

   inline size_t streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   )
   {

      Path document = path;

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
      std::string contentType,
      Size contentLength
   )
   {
      
      stringstream stream;
      stream <<
         "HTTP/2.0 200 OK" << "\r\n" <<
         "Connection: keep-alive\r\n" <<
         "Content-Type: " <<
            contentType << "\r\n";
         if (contentLength > 0)
            stream <<
               "Content-Length: " <<
               contentLength << "\r\n";
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
