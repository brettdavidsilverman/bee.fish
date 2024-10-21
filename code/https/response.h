#ifndef BEE_FISH_HTTPS__RESPONSE_H
#define BEE_FISH_HTTPS__RESPONSE_H

#include <filesystem>

#include "app.h"
#include "error-app.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

   class Session;
   
   class Response {
   protected:
      const bool _log = true;

      Session* _session;
      Size _bytesTransferred = 0;
      ssize_t _contentLength = 0;
      const Size _pageSize = getPageSize();
      App*      _app = nullptr;
   public:
      Response(
         Session* session
      ) :
         _session(session)
      {
      }
      
      virtual void handleResponse()
      {

         ResponseHeaders headers(_session);

         App* app = nullptr;

         try {
            for ( auto factory : appFactories )
            {

               app = factory->create(
                  _session,
                  headers
               );
          
               app->handleResponse();
            
               if (app->status() != -1)
                  break;
  
               delete app;
            
               app = nullptr;

            }
         }
         catch (std::exception& exception)
         {
            
            App* errorApp = new ErrorApp(_session, headers, app, exception);
            
            errorApp->handleResponse();
            
            if (app)
               delete app;
               
            app = errorApp;
            
         }

         if (app)
         {
            if (_log) {
               clog << now() 
                  << " " << ipAddress()
                  << " " << app->status()
                  << " " << app->statusText()
                  << " Served by "
                  << app->name()
                  << endl;
            }
                 
           
            _app = app;

            _contentLength = _app->contentLength();
            
            if (_contentLength == -1) {
               headers.erase("content-length");
               headers.replace(
                  "transfer-encoding",
                  "chunked"
               );
            }
            else
               headers.replace(
                  "content-length",
                  std::to_string(_contentLength)
               );
               
         
            write();
            
            end();
            
         }
         
         
            
      }
      
      
      virtual ~Response()
      {
         if (_app) {
            delete _app;
            _app = nullptr;
         }
      }
      
      // Defined in session.h
      const BString& ipAddress() const;
      
      // Defined in session.h
      void closeOrRestart();
      
      void end() 
      {
         cerr << "response.h " 
              <<_app->bytesTransferred() << " of "
              << _app->contentLength()
              << endl;

         bool end = !_app ||
            ( _app->bytesTransferred() ==
             (Size) _app->contentLength() ) ||
             _app->contentLength() == -1;
            
         if ( end )
         {
            cerr << "response.h end " << endl;
            
            if (_app)
            {
               delete _app;
               _app = nullptr;
            }
            
            closeOrRestart();

         }
         
      }
     
      // Defined in response-stream.h
      void write();
      
      App* app()
      {
         return _app;
      }

   };
   
}

#endif