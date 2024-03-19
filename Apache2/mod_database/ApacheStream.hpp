#ifndef BEE_FISH_WEB_SERVER__APACHE_STREAM_URI
#define BEE_FISH_WEB_SERVER__APACHE_STREAM_URI

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

using namespace std;

namespace BeeFishWebServer {

   class ApacheStream :
      private std::streambuf,
      public std::ostream
   {
   private:
      Size _pageSize;
      char* _buffer;
      Size _count;
      request_rec * _r;
      
   public:
      ApacheStream(request_rec * r) : std::ostream(this)
      {
         _pageSize = getPageSize();
         _buffer = new char[_pageSize];
         _count = 0;
         _r = r;
      }
      
      virtual ~ApacheStream()
      {
         delete[] _buffer;
      }
      
      void put(int c)
      {
         _buffer[_count++] = (char)c;
         
         if (_count == _pageSize)
            flush();
      }
      
      virtual void flush()
      {
          std::ostream::flush();
          
          if (_count == 0)
             return;
             
          ap_rwrite(
             _buffer,
             _count,
             _r 
          );
          
          _count = 0;
      }
      
   private:
      int overflow(int c) override
      {
        put(c);
        return 0;
      }
   };

}

#endif