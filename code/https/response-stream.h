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
      ResponseStream(Session* session, App* app) : std::ostream(this)
      {
         _pageSize = getPageSize();
         _buffer = new char[_pageSize];
         _count = 0;
         _bytesTransferred = 0;
         _session = session;
         setChunkedEncoding(app);
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
      
      void writeHeaders(App* app)
      {
         _writingHeaders = true;
         
         ResponseStream& stream = *this;
         
         stream
            << "HTTP/1.1 " 
            << app->status()
            << " "
            << app->statusText()
            << "\r\n";
            
         const ResponseHeaders& headers =
            app->responseHeaders();
            
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
      
      void writeContent(App* app)
      {
         
         
         if (app->serve() == App::SERVE_JSON)
         {
            JSONPath path(app->_bookmark);
            *this << path;
            flush();
            return;
         }
         
         Size bytesTransferred = 0;
         Size pageSize = getPageSize();
         ssize_t _contentLength = app->contentLength();
         Size length = _contentLength;
         std::string buffer(pageSize, '\0');
         
         while ((ssize_t)bytesTransferred < _contentLength)
         {
            if ((_contentLength - bytesTransferred) 
                  > pageSize)
               length = pageSize;
            else
               length = _contentLength - bytesTransferred;
                     
            if (!length)
               break;
               
            switch (app->serve())
            {
               case App::SERVE_DATA:
               {
            
                  Size pageIndex =
                     bytesTransferred  /
                     pageSize;

                  std::string data =
                     app->_bookmark[pageIndex].getData();
                  
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
                        app->_content.data()
                        + bytesTransferred 
                     ),
                     length
                  );
                  break;
               }
               case App::SERVE_FILE:
               {
                  ifstream input(app->_filePath);
                     
                  input.seekg(
                     bytesTransferred
                  );
                  
                  input.read(
                     buffer.data(),
                     length
                  );
                  
                  input.close();
               
                  break;
               }
               default:
               {
                  throw std::logic_error("Invalid Serve enum value");
               }
            }
             
            write(
               buffer.data(),
               length
            );
         
            bytesTransferred += length;
             
         }
      
          
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
      
      void setChunkedEncoding(App* app)
      {
         const ResponseHeaders& headers =
            app->responseHeaders();
            
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
   void Response::write(App* app)
   {

      ResponseStream stream(_session, app);
         
      stream.writeHeaders(app);
         
      stream.writeContent(app);
        
      stream.flush();
      
      _bytesTransferred =
         stream.bytesTransferred();
   }
   
}

#endif