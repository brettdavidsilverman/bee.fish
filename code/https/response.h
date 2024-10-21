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
      
      string _headers;
      
      Size _headersLength = 0;
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
            
            if (_contentLength == -1)
               headers.erase("content-length");
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
            
            _headers.clear();
            if (_app)
            {
               delete _app;
               _app = nullptr;
            }
            
            closeOrRestart();

         }
         
      }
     

      void write()
      {

         ResponseStream stream(_session);
         
         _app->write(stream);
         
         stream.flush();
         
         _bytesTransferred =
            stream.bytesTransferred();
      }
      
      
      App* app()
      {
         return _app;
      }
   
      class ResponseStream :
         private std::streambuf,
         public std::ostream
      {
      private:
         Size _pageSize;
         char* _buffer;
         Size _count;
         Size _bytesTransferred;
         Session* _session;
         
      public:
          ResponseStream(Session* session) : std::ostream(this)
      {
         _pageSize = getPageSize();
         _buffer = new char[_pageSize];
         _count = 0;
         _bytesTransferred = 0;
         _session = session;
      }
      
      virtual ~ResponseStream()
      {
         delete[] _buffer;
      }
      
      void put(int c)
      {
         _buffer[_count++] = (char)c;
         
         if (_count == _pageSize)
            flush();
      }
      
      Size bytesTransferred() const
      {
         return _bytesTransferred;
      }
      
      // Definef in session.h
      virtual void flush();
      
      private:
         int overflow(int c) override
         {
            put(c);
            return 0;
         }
      };

   };
   

   
}

#endif