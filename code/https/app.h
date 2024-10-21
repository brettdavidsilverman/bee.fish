#ifndef BEE_FISH_HTTPS__APP_H
#define BEE_FISH_HTTPS__APP_H
#include <vector>
#include <filesystem>
#include "../web-request/web-request.h"
#include "response-headers.h"
#include "authentication.h"

using namespace std;
using namespace std::filesystem;
using namespace BeeFishWeb;
using namespace BeeFishJSON;

namespace BeeFishHTTPS {

   class Session;
   
   class App : public Authentication {
   protected:
      Session* _session;
   public:
      
      enum Serve {
         SERVE_FILE,
         SERVE_CONTENT,
         SERVE_DATA,
         SERVE_JSON
      } _serve = SERVE_CONTENT;

      int _status = -1;
      string _statusText = "Ok";
      ResponseHeaders& _responseHeaders;
      std::string _content;
      Path _bookmark;
      path   _filePath;
      ssize_t _contentLength = 0;
      Size _bytesTransferred = 0;
   public:
      App(
         Session* session,
         ResponseHeaders& responseHeaders
      ) :
         Authentication(session),
         _session(session),
         _responseHeaders(responseHeaders)
      {
      }
      
      virtual ~App()
      {
      }
      
      virtual void handleResponse() = 0;

      // Defined in session.h
      bool parseWebRequest(BeeFishParser::Parser& parser);

      // Defined in session.h
      WebRequest* request();

      virtual int status()
      {
         return _status;
      }
      
      virtual string statusText() {
         return _statusText;
      }

      virtual const ResponseHeaders& responseHeaders() const
      {
         return _responseHeaders;
      }
      
      virtual string content()
      {
         return _content;
      }
      
      virtual Serve serve() {
         return _serve;
      }
      
      virtual path filePath()
      {
         return _filePath;
      }
      
      virtual BString name()
      {
         return "App base class";
      }
      
      Session* session()
      {
         return _session;
      }
      
      Size bytesTransferred() {
         return _bytesTransferred;
      }
      
      void redirect(
         BString path,
         bool permanent,
         BString from = ""
      )
      {
         if (permanent)
            _status = 301;
         else
            _status = 307;
            
         _responseHeaders.replace(
            "connection", "keep-alive"
         );
         
         _responseHeaders.replace(
            "location", path
         );
         
         if (!permanent) {
            _responseHeaders.replace(
               "cache-control",
               "no-store, max-age=0"
            );
         }
         
         _responseHeaders.replace(
            "content-type",
            "text/plain; charset=utf-8"
         );
         
         if (path != from)
         {
             
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=;path=/;max-age=0;")
            
            );
            
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=") + from +
               BString(";path=/;")
            );
         }
         
         _content = "redirecting...";
         
         _serve = App::SERVE_CONTENT;
         

      }

      path getFilePath(const BString& requestPath) const
      {
            
         BString fullWebRequestPath =
            BString(WWW_ROOT_DIRECTORY) +
            requestPath;
               
         path filePath = canonical(
            path(fullWebRequestPath.str())
         );
            
         if (is_directory(filePath))
         {
            try
            {
               BString indexPath =
                  fullWebRequestPath +
                  BString("index.html");
                  
               filePath =
                  canonical(
                     path(indexPath.str())
                  );
            }
            catch(filesystem_error& err)
            {
            }
         }
         
         return filePath;
      }
      
      virtual void write(ostream& stream)
      {
         stream 
            << "HTTP/1.1 " 
            << _status 
            << " "
            << _statusText
            << "\r\n";
            
         for (auto pair : _responseHeaders)
         {
            stream
               << pair.first
               << ": "
               << pair.second.str()
               << "\r\n";
         }
            
         stream << "\r\n";
         
         if (serve() == App::SERVE_JSON)
         {
            JSONPath json(_bookmark);
            json.write(stream);
            stream << "\r\n";
            return;
         }
  
         
         Size pageSize = getPageSize();
         ssize_t _contentLength = contentLength();
         Size length = _contentLength;
         std::string buffer(pageSize, '\0');
         
         while ((ssize_t)_bytesTransferred < _contentLength)
         {
            if ((_contentLength - _bytesTransferred) 
                  > pageSize)
               length = pageSize;
            else
               length = _contentLength - _bytesTransferred;
                     
            if (!length)
               break;
               
            switch (serve())
            {
               case App::SERVE_DATA:
               {
            
                  Size pageIndex =
                     _bytesTransferred  /
                     pageSize;

                  BeeFishDatabase::Data data =
                     _bookmark[pageIndex].getData();
                  
                  length = data.size();

                  memcpy(buffer.data(), data.data(), length);
                  
                  break;
               }
               case App::SERVE_CONTENT:
               {
                 
                  memcpy(
                     buffer.data(),
                     (const Byte*)
                        (
                           _content.data()
                           + _bytesTransferred 
                        ),
                     length
                  );
                  break;
               }
               case App::SERVE_FILE:
               {
                  ifstream input(_filePath);
                     
                  input.seekg(
                     _bytesTransferred
                  );
                  
                  input.read(
                     buffer.data(),
                     length
                  );
                  
                  input.close();
               
                  break;
               }
               default:
                  throw std::logic_error("Invalid Serve enum value");
             }
             
             stream.write(
                buffer.data(),
                length
             );
             _bytesTransferred += length;
             
          }
          
          
          
      }
   
      virtual ssize_t contentLength()
      {
         switch (serve())
         {
         case App::SERVE_FILE:
            return file_size(_filePath);
         case App::SERVE_CONTENT:
            return _content.size();
         case App::SERVE_DATA:
            return _contentLength;
         case App::SERVE_JSON:
            return -1;
         default:
            return _contentLength;
         }
         
      }
      
   };
   
   
   class Factory
   {
   public:
      Factory()
      {
      }
      
      virtual ~Factory()
      {
      }
      
      virtual App* create(
         Session* session,
         ResponseHeaders& headers
      ) = 0;
      
   };
   
   template<class T>
   class AppFactory : public Factory
   {
   public:
      AppFactory()
      {
      }
      
      virtual App* create(Session* session, ResponseHeaders& headers)
      {
         return new T(session, headers);
      }
   };
   
   class AppFactories :
      public vector<Factory*>
   {
   public:
      template<class T>
      void add()
      {
         push_back(new AppFactory<T>());
      }
      
      ~AppFactories()
      {
         for ( auto it = begin();
                    it != end();
                    ++it )
         {
            Factory* factory = *it;
            delete factory;
         }
      }
      
   };
   
   inline AppFactories appFactories;
   

}

#endif