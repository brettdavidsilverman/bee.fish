#ifndef BEE_FISH_B_STRING__STREAM_H
#define BEE_FISH_B_STRING__STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <functional>

#include "../power-encoding/power-encoding.h"
#include "../Miscellaneous/Miscellaneous.hpp"

using namespace BeeFishPowerEncoding;
using namespace BeeFishMisc;

namespace BeeFishBString {

   typedef std::vector<unsigned char> Bytes;

   class BStream :
      private std::streambuf,
      public std::ostream
   {
   public:
      typedef std::function<void(const std::string& buffer)> OnBuffer;
      OnBuffer _onbuffer = nullptr;
      size_t _bufferSize;
   protected:
      Bytes  _bytes;
      size_t _totalSize = 0;
   
   public:
      
      BStream(
         size_t bufferSize = getPageSize()
      ) :
         std::ostream(this),
         _bufferSize(bufferSize)
      {
         _bytes.reserve(_bufferSize);
      }

      BStream(const BStream& copy) :
         std::ostream(this),
         _onbuffer(copy._onbuffer),
         _bufferSize(copy._bufferSize),
         _bytes(copy._bytes)
         
      {
         _bytes.reserve(_bufferSize);
      }

      virtual ~BStream()
      {
      }


      int overflow(int c) override
      {
         push_back((unsigned char)c);
         return 0;
      }

      size_t size()
      {
         return _bytes.size();
      }
      
      virtual void push_back(unsigned char c)
      {
         _bytes.push_back(c);
         if (size() >= _bufferSize)
            onBuffer();
      }      

      virtual void flush()
      {
         if (size() > 0)
            onBuffer();
      }

      virtual void setOnBuffer(OnBuffer onbuffer)
      {
         _onbuffer = onbuffer;
      }

      virtual void write(const std::string& data) 
      {
         write(data.data(), data.size());
      }

      virtual void write(const char* data, size_t size)
      {
         for (size_t i = 0; i < size; ++i)
         {
            push_back((char)data[i]);
         }
      }

      inline friend BStream& operator << (BStream& out, std::string value)
      {
         out.write(value);
         return out;
      }

      inline friend BStream& operator << (BStream& out, const char* value) 
      {
         for (const char *pc = value; *pc != 0; ++pc) {
            out.push_back(*pc);
         }
         return out;
      }

      inline friend BStream& operator << (BStream& out, const BeeFishBString::BString& bstring) 
      {
         (ostream&)out << bstring;
         return out;
      }

      inline friend BStream& operator << (BStream& out, int value) 
      {
         (ostream&)out << value;
         return out;
      }

      inline friend BStream& operator << (BStream& out, size_t value) 
      {
         (ostream&)out << value;
         return out;
      }

      virtual size_t totalSize() {
         return _totalSize;
      }

   protected:

      virtual void onBuffer() {

         _totalSize += size();


         if (_onbuffer) {

            std::string data((char*)_bytes.data(), size());

            _onbuffer(data);

         }


         clear();
      } 

      virtual void clear() {
         _bytes.clear();
         _bytes.reserve(_bufferSize);
      }



   };

   
}

#endif