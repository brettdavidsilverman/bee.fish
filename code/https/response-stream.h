#ifndef BEE_FISH_HTTPS__RESPONSE_STREAM_H
#define BEE_FISH_HTTPS__RESPONSE_STREAM_H

#include <filesystem>
#include "session.h"
#include "app.h"
#include "response.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

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
      bool _chunkedEncoding = false;
      bool _writingHeaders = true;
         
   public:
      ResponseStream(Session* session) : std::ostream(this)
      {
         _pageSize = getPageSize();
         _buffer = new char[_pageSize];
         _count = 0;
         _bytesTransferred = 0;
         _session = session;
         setChunkedEncoding();
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
      
      void writeHeaders()
      {
         _writingHeaders = true;
         
         ResponseStream& stream = *this;
         
         const App* _app = app();
         
         stream
            << "HTTP/1.1 " 
            << _app->status()
            << " "
            << _app->statusText()
            << "\r\n";
            
         const ResponseHeaders& headers =
            responseHeaders();
            
         for (auto pair : headers)
         {
            stream
               << pair.first
               << ": "
               << pair.second.str()
               << "\r\n";
         }
            
         stream << "\r\n";
         
         flush();
        
        _writingHeaders = false;
      }
      
      void writeContent()
      {
         app()->write(*this);
         
         flush();
      }
      
      bool chunkedEncoding()
      {
         return _chunkedEncoding;
      }
      
      bool writingHeaders()
      {
         return _writingHeaders;
      }
      
      const ResponseHeaders&
      responseHeaders()
      {
         const ResponseHeaders& headers =
            app()->
            responseHeaders();
          return headers;
      }
     
     
      App* app()
      {
         return
            _session->
            response()->
            app();
      }
   
      void setChunkedEncoding()
      {
         const ResponseHeaders& headers =
            responseHeaders();
            
         _chunkedEncoding =
            ( headers["transfer-encoding"]
              == "chunked" );
            
      }
     
      virtual void flush()
      {
         std::ostream::flush();
          
         if (chunkedEncoding() && 
             !writingHeaders())
         {
            std::stringstream stream;
            stream << std::hex << std::uppercase << _count << std::dec << "\r\n";
            std::string str = stream.str();
            _write(str.data(), str.size());
         }
         
         if (_count > 0)
         {
            _write(_buffer, _count);
            _bytesTransferred += _count;
         }
         
         if (
             chunkedEncoding() && 
             !writingHeaders())
         {
            std::string str = "\r\n";
            _write(str.data(), str.size());
         }
        
         _count = 0;
     
      
      }
      
      void _write(
         const char* buffer,
         Size size
      )
      {

         boost::asio::write(
            *_session,
            boost::asio::buffer(
               buffer,
               size
            )
         );
         
      }
  
   private:
      int overflow(int c) override
      {
         put(c);
         return 0;
      }
   };

  
   // Declared in response.h
   void Response::write()
   {

      ResponseStream stream(_session);
         
      stream.writeHeaders();
         
      stream.writeContent();
        
      stream.flush();
      
      _bytesTransferred =
         stream.bytesTransferred();
   }
   
}

#endif