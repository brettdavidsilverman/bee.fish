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
#include "../power-encoding/power-encoding.h"


using namespace BeeFishPowerEncoding;


namespace BeeFishDatabase {

   typedef uint8_t Byte;

   struct Data
   {
   public:
      size_t _size = 0;
      char* _data = nullptr;
      
      Data(size_t size = 0)
      {
         _size = size;
         if (_size)
            _data = new char[size];
      }
      
      Data(const Data& source)
      {
         _size = source._size;
         if (_size)
         {
            _data = new char[_size];
            memcpy(_data, source._data, _size);
         }
      }
      
      Data(const char* data, size_t size)
      {
         _size = size;
         if (_size)
         {
            _data = new char[_size];
            memcpy(_data, data, _size);
         }
      }
      
      Data(const BString& data)
      {
         _size = data.size();
         if (_size)
         {
            _data = new char[_size];
            memcpy(_data, data.data(), _size);
         }
      }

      Data& operator = (const Data& rhs)
      {
         if (_data)
            delete[] _data;
            
         _data = nullptr;
         
         _size = rhs._size;
         
         if (_size) {
           _data = new char[_size];
           memcpy(_data, rhs.data(), _size);
         }
         
         return *this;
      }
      
      ~Data()
      {
         if (_data) {
            delete[] _data;
            _data = nullptr;
         }
      }
   
      template<typename T>
      Data& operator = (const T& rhs) 
      {

         if (_size < sizeof(T))
            throw runtime_error("Data buffer size to small");

         memcpy(data(), &rhs, sizeof(T));

         return *this;
         
      }

      Data& operator = (const std::string& rhs) 
      {
         
         if (_size < sizeof(rhs.size()))
            throw runtime_error("Data buffer size to small");

         _size = rhs.size();

         memcpy(data(), rhs.data(), _size);

         return *this;
         
      }

      Data& operator = (const char* rhs) 
      {
         return operator = (std::string(rhs));
      }


      char* data() {
         return _data;
      }

      const char* data() const {
         return _data;
      }

      size_t size() const {
         return _size;
      }

      std::string str() const
      {
         return std::string(data(), _size);
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
      
      
      bool operator == (const Data& rhs) const
      {
         if (_size != rhs._size)
            return false;
      
         int result = 
            memcmp(
               _data, 
               rhs._data, 
               _size
            );
       
         return (result == 0);
      }
      
      bool operator != (const Data& rhs) const 
      {
         if (_size == rhs._size)
         {
            int result = 
               memcmp(
                  _data, 
                  rhs._data, 
                  _size
               );
            return (result != 0);

          }

          return true;
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
      

      inline static std::string fromRandom(
         size_t byteCount
      )
      {
         return createRandom(byteCount);
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
      
         
   };
   
   
   
}

#endif
