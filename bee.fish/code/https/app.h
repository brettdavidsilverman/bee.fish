#ifndef BEE_FISH_HTTPS__APP_H
#define BEE_FISH_HTTPS__APP_H
#include <vector>
#include <filesystem>
#include "../web-request/web-request.h"
#include "response-headers.h"
#include "authentication.h"
#include "../b-script/b-script.h"

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
         SERVE_DATA
      } _serve = SERVE_CONTENT;

      int _status = -1;
      string _statusText = "OK";
      ResponseHeaders& _responseHeaders;
      std::string _content;
      Path<PowerEncoding> _bookmark;
      path   _filePath;
      size_t _contentLength = 0;

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
            "text/plain; charset=UTF-8"
         );
         
         if (path != from)
         {
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=;path=/;max-age=0")
            
            );
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=") + from +
               ";path=/"
            );
         }
         
         _content = "redirecting...";
         
         _serve = App::SERVE_CONTENT;

      }

      path getFilePath(const BString& requestPath) const
      {
            
         BString fullWebRequestPath =
            BString(FILE_SYSTEM_PATH) +
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
                  "index.html";
                  
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