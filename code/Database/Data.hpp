#ifndef BEE_FISH__DATABASE__DATA_H
#define BEE_FISH__DATABASE__DATA_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <string>
#include <iomanip>

#ifdef SERVER
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif
#include "Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../PowerEncoding/PowerEncoding.hpp"

using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {

   typedef uint8_t Byte;

   struct Data
   {
   protected:
      size_t _size = 0;
      char _data[];
   public:
 /*
      Data(char* data, size_t size)
      :
         _data(data),
         _size(size)
      {
      }


      virtual ~Data() {
      }
      
      template<typename T>
      Data(vector<T>& source)
      :
         _data(source.data()),
         _size(source.size() * sizeof(T))
      {
      }

      Data(void* data, size_t size)
      :
         _data((char*)data), _size(size)
      {
      }
      
      Data(char* source)
      :
         Data(source, strlen(source))
      {
         
      }
      
      Data(std::string string) : Data(string.data(), string.size()) {
         
      }

      // Implemented in misc.h

      Data(const string& source);
      operator string() const;
      */


      template<typename T>
      Data& operator = (const T& rhs) {

         if (_size < sizeof(T))
            throw runtime_error("Data buffer size to small");

         memcpy(data(), &rhs, sizeof(T));

         return *this;
         
      }

      char* data() {
         return &_data[0];
      }

      const char* data() const {
         return &_data[0];
      }

      size_t size() const {
         return _size;
      }

      void setSize(size_t size) {
         _size = size;
      }

      template<typename T>
      operator const T&() const
      {
         const T* destination =
            (const T*)_data;
         
         return *destination;
      }

      template<typename T>
      operator T&()
      {
         T* destination =
            (T*)_data;
         
         return *destination;
      }

      template<typename T>
      operator T() const
      {
         return T(_data);
      }

      bool operator == (const Data& rhs) const {
         if (_size != rhs._size)
            return false;
         int result = memcmp(_data, rhs._data, _size);
         return (result == 0);
      }
      
      //operator string() const;

      string toHex() const;
      

      // defined in base64.h
      // included from string.h
      string toBase64() const;

      // defined in base64.h
      // included from string.h
      static Data fromBase64
      (const string& data);

#ifdef SERVER
      
      string md5() const;

      // sha3_512
      string sha3() const;
      

      inline static Data fromRandom(
         size_t byteCount
      )
      {
         unsigned char buffer[byteCount];

         int rc = RAND_bytes(
            buffer,
            byteCount
         );
      
         // unsigned long err = ERR_get_error();

         if (rc != 1)
         {
            // RAND_bytes failed
            throw runtime_error("Random bytes failed");
         
         }
      
         return Data(buffer, byteCount, true);
      }
#endif

      friend ostream& operator <<
      (ostream& out, const Data& data)
      {
         data.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         std::ios_base::fmtflags f( out.flags() );
         out << std::setw(2) << std::setfill('0') <<  std::uppercase << std::right << std::hex;
         for (size_t i = 0; i < _size; ++i)
         {
            Byte byte = _data[i];
            out << (int)byte;
            out << ' ';
         }
         
         out.flags(f);

      }
      
      // Implemented in misc.h
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const Data& data
      );

      static Size getPageSize() {
         return PAGE_SIZE;
      }
         
   };
   
   
   
}

#endif
