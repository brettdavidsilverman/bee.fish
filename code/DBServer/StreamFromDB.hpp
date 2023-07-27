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
              
      if (!jsonPath.hasData())
         throw runtime_error("JSON path has no data");

      Size size = 0;

      JSONValue* value = (JSONValue*)
         jsonPath.getData().data();

      void* data = value->data();

      switch (value->_type)
      {
         case JSONType::INT64:
         {
            stringstream stream;
            //stream << *(int64_t*)(value->_data);
            int64_t* i = (int64_t*)data;
            stream << *i;
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
            std::string str((char*)data, value->_size);
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
            size += output.write("{");
            if (!jsonPath.isDeadEnd())
            {
               Stack stack;
               string key = jsonPath.min<string>(stack);
               string last = jsonPath.max<string>();

               string label;
               do {
                  label = "\"" + escape(key) + "\":";

                  size += output.write(
                     label
                  );

                  size += streamJSONFromDB(
                     output,
                     jsonPath[key]
                  );

                  if (key != last)
                     size += output.write(",");
               }
               while(jsonPath.next(stack, key));
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

               Size index = jsonPath.min<Size>(stack);
               Size max = jsonPath.max<Size>();

               do
               {
                  size += streamJSONFromDB (
                     output,
                     jsonPath[index]
                  );

                  if (index != max)
                     size += output.write(",");
   
               }
               while(jsonPath.next(stack, index));

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

      const Size size() const {
         return
            _position +
            _pageCount * pageSize;
      }

   };
*/
}

#endif
