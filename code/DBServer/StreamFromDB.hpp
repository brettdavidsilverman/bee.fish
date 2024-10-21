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
  
   inline string getTabs(Size tabs)
   {
       return string(tabs * TAB_SPACES, ' ');
   }
   
   inline Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path path
   );

   inline Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      MinMaxPath path,
      int tabs = 0
   );

   inline bool writeHeaders(
      BeeFishWeb::WebRequest& output,
      std::string contentType,
      Size contentLength
   );

   inline Size streamFromDB (
      BeeFishWeb::WebRequest& output,
      Path path
   )
   {
      const std::string contentType =
         "application/json; charset=utf-8";
         
      if (path.isDeadEnd())
      {
         throw std::runtime_error("Path is dead end");
      }
/*
      if (path.contains("document"))
      {
         Size contentLength;
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
*/
      //if (contentType == "application/json; charset=utf-8")
      {
         if (!writeHeaders(
               output,
               contentType,
               0))
         {
            return 0;
         }

         return streamJSONFromDB(
//            output, path["JSON"]
            output, path
         );
      }


      return 0;
   }

   inline Size streamJSONFromDB (
      BeeFishWeb::WebRequest& output,
      MinMaxPath path,
      int tabs
   )
   {

      Size size = 0;
      Type type;
      
      if (path.isDeadEnd())
          type = Type::UNDEFINED;
      else {
         type =
            path.value<Type>();
      }
      cerr << "STREAMFROMDB.HPP" << ":" << type << endl;
      
      path = path[type];

      string value;

      if (path.hasData())
         path.getData(value);
    
      switch (type)
      {
         case Type::OBJECT:
         {            
            Size count = atol(value.c_str());
            if (count == 0)
               size += output.write("{");
            else
               size += output.write("{\r\n");
cerr << "STREAMFROMDB.HPP: size: " << size << endl;
            string key;
            Stack stack;
            Size i = 0;
            
            path = path[Type::OBJECT];
            
            while(path.next(stack, key))
            {
               size += output.write(getTabs(tabs + 1));
               
               string label = "\"" + escape(key) + "\": ";

               size += output.write(label);

               size += streamJSONFromDB(
                  output,
                  path[key],
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
         case Type::NULL_:
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
            Size count;
            stream << value;
            stream >> count;
            size += output.write("[");
            
            if (count > 0)
               size += output.write("\r\n");
            
            path = path[Type::ARRAY];
            
            for (Size index = 0;
                 index < count;
                 ++index)
            {

               if (path[index].hasData())
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

   inline Size streamDocumentFromDB (
      BeeFishWeb::WebRequest& output,
      Path path
   )
   {

      MinMaxPath document = path;

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
