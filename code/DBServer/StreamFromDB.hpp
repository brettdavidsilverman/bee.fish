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
      BeeFishWeb::Path jsonPath
   )
   {

      Size size = 0;

      JSONType type =
         jsonPath.value<JSONType>();

cerr << "$$$$$$$ type: " << type << endl;

      assert(jsonPath.contains(type));

      Path path = jsonPath[type];

      
      assert(path.hasData());
      string value;
      path.getData(value);

      switch (type)
      {
         case JSONType::OBJECT:
         {
            size += output.write("{");
            Size count = atol(value.c_str());
            string key;
            Stack stack;
            Path object = path;
            int i = 0;
            while (object.next(stack, key))
            {
               string label = "\"" + escape(key) + "\":";

               size += output.write(label);
               Path value = object[key];

               size += streamJSONFromDB(
                  output,
                  value
               );

               if (++i < count)
                  size += output.write(",");

            }

            size += output.write("}");
            break;
         }
         case JSONType::INT64:
         {
            size += output.write(value);
            break;
         }
         case JSONType::UINT64:
         {
            size += output.write(value);
            break;
         }
         case JSONType::DOUBLE:
         {
            size += output.write(value);
            break;
         }
         case JSONType::STRING:
         {
            string str = "\"" + escape(value) + "\"";
            size += output.write(str);
            break;
         }
         case JSONType::BOOL:
         {
            size += output.write(value);
            break;
         }
         case JSONType::_NULL:
         {
            string str = "null";
            size += output.write(str);
            break;
         }
         case JSONType::ARRAY:
         {
            size += output.write("[");

            Size count = atol(value.c_str());
cerr << "###### Value: " << value << endl;

            for (Size index = 0;
                 index < count;
                 ++index)
            {
cerr << "💜Index: " << index << ":" << path.contains(index) << endl;

               if (path.contains(index)) {
                  size += streamJSONFromDB (
                     output,
                     path[index]
                  );
               }

               if ((index + 1) < count)
                  size += output.write(", ");
   
            }

            size += output.write("]");

            break;
         }
         default:
         {
            size += output.write(value);
         }
         
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
