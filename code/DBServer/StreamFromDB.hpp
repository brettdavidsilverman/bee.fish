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

   inline Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   );

   inline Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   );

   inline bool writeHeaders(
      BeeFishWeb::WebRequest& output,
      std::string contentType,
      Size contentLength
   );

   inline Size streamFromDB (
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

   inline Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> jsonPath
   )
   {

      assert(jsonPath.hasData());

      Size size = 0;

      JSONData* jsonData = (JSONData*)
         jsonPath.getData().data();

      void* data = jsonData->data();

      switch (jsonData->_type)
      {
         case JSONType::INT64:
         {
            stringstream stream;
            stream << *(int64_t*)data;
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::UINT64:
         {
            stringstream stream;
            stream << *(uint64_t*)data;
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::DOUBLE:
         {
            stringstream stream;
            
            stream << std::setprecision(16)  << *(double*)data;
            string str = stream.str();
            size += output.write(str);
            break;
         }
         case JSONType::STRING:
         {
            std::string str((char*)data, jsonData->_size);
            str = "\"" + escape(str) + "\"";
            size += output.write(str);
            break;
         }
         case JSONType::BOOL:
         {
            string str = (*(bool*)data
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
            cerr << "OBJECT ";

            Size count = *(Size*)data;
            cerr << count << endl;
            size += output.write("{");

            Stack stack;
            string key;
            int i = 0;
            while (jsonPath.next(stack, key))
            {

               string label = "\"" + escape(key) + "\":";
cerr << label << jsonPath << endl;
               size += output.write(
                  label
               );

               size += streamJSONFromDB(
                  output,
                  jsonPath[key]
               );

                  
               if (++i != count)
                  size += output.write(",");

               key.clear();

            }

            size += output.write("}");
            break;
         }
         case JSONType::ARRAY:
         {
            size += output.write("[");

            if (!jsonPath.isDeadEnd())
            {

               Stack stack;

               Size index;
               Size last = jsonPath.max<Size>();

               while(jsonPath.next(stack, index))
               {
                  size += streamJSONFromDB (
                     output,
                     jsonPath[index]
                  );

                  if (index != last)
                     size += output.write(",");
   
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

   inline Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path<Database::Encoding> path
   )
   {

      Path document = path;

      // Output the document content
      Size pageIndex  = 0;
      Size min = document.min<Size>();
      Size max = document.max<Size>();
      Size byteCount = 0;

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
}

#endif
