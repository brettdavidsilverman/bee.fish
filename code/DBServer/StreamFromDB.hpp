#ifndef BEEFISH__WEBDB__STREAM_FROM_DB
#define BEEFISH__WEBDB__STREAM_FROM_DB

#include "../Parser/Parser.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "Config.hpp"


namespace BeeFishDBServer {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;
   using namespace BeeFishWebDB;
   using namespace BeeFishJSON;
  // using namespace BeeFishScript
  
   typedef BeeFishDatabase::Path<Database::Encoding> Path;
   
   inline string getTabs(BeeFishDatabase::Size tabs)
   {
       return string(tabs * TAB_SPACES, ' ');
   }
   
   inline BeeFishDatabase::Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path path
   );

   inline BeeFishDatabase::Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path path,
      int tabs
   );

   inline bool writeHeaders(
      BeeFishWeb::WebRequest& output,
      std::string contentType,
      BeeFishDatabase::Size contentLength
   );

   inline BeeFishDatabase::Size streamFromDB (
      BeeFishWeb::WebRequest& output,
      Path path,
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
            output, path["JSON"], 0
         );
      }

      if (path.contains("document"))
      {
         BeeFishDatabase::Size contentLength;
         path["document"].getData(contentLength);

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

   inline BeeFishDatabase::Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      Path path,
      int tabs
   )
   {

      BeeFishDatabase::Size size = 0;

      assert(!path.isDeadEnd());

      Type type =
         path.value<Type>();
  
      assert(path.contains(type));

      path << type;

      string value;

      assert(path.hasData());

      path.getData(value);

      switch (type)
      {
         case Type::OBJECT:
         {            
            size += output.write("{");
            
            stringstream stream;
            BeeFishDatabase::Size count;
            stream << value;
            stream >> count;

            if (count > 0) {
                size += output.write("\r\n");
            }
            
            Path
               table = path[OBJECT_TABLE];
               
            Path
               keys = path[OBJECT_KEYS];

            for (int i = 0; i < count;)
            {
               string key;
               table[i].getData(key);

               assert(keys.contains(key));

               size += output.write(getTabs(tabs + 1));
               
               string label = "\"" + escape(key) + "\": ";

               size += output.write(label);

               size += streamJSONFromDB(
                  output,
                  keys[key],
                  tabs + 1
               );

               if (++i < count) {
                  size += output.write(",");
                  size += output.write("\r\n");
               }

            }
            
            if (count > 0) {
               size += output.write("\r\n");
               size += output.write(getTabs(tabs));
            }
            size += output.write("}");
            break;
         }
         case Type::NUMBER:
         {
            size += output.write(value);
            break;
         }
         case Type::STRING:
         {
            string str = "\"" + escape(value) + "\"";
            size += output.write(str);
            break;
         }
         case Type::BOOLEAN:
         {
            size += output.write(value);
            break;
         }
         case Type::_NULL:
         {
            string str = "null";
            size += output.write(str);
            break;
         }
         case Type::UNDEFINED:
         {
            string str = "undefined";
            size += output.write(str);
            break;
         }
         case Type::ARRAY:
         {
            stringstream stream;
            BeeFishDatabase::Size count;
            stream << value;
            stream >> count;
            size += output.write("[");
            
            if (count > 0)
               size += output.write("\r\n");
            
            for (BeeFishDatabase::Size index = 0;
                 index < count;
                 ++index)
            {

               if (path.contains(index))
               {
                  size += output.write(getTabs(tabs + 1));
                  size += streamJSONFromDB (
                     output,
                     path[index],
                     tabs + 1
                  );
               }

               if ((index + 1) < count) {
                  size += output.write(",");
                  size += output.write("\r\n");
               }
   
            }

            if (count > 0) {
               size += output.write("\r\n");
               size += output.write(getTabs(tabs));
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

   inline BeeFishDatabase::Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path path
   )
   {

      Path document = path;

      // Output the document content
      BeeFishDatabase::Size pageIndex  = 0;
      BeeFishDatabase::Size min = document.min<BeeFishDatabase::Size>();
      BeeFishDatabase::Size max = document.max<BeeFishDatabase::Size>();
      BeeFishDatabase::Size byteCount = 0;

      for ( pageIndex = min;
            pageIndex <= max;
            ++pageIndex )
      {

         std::string data;

         Path page =
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
      BeeFishDatabase::Size contentLength
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
