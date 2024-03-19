#ifndef BEE_FISH_B_STRING__DATA_H
#define BEE_FISH_B_STRING__DATA_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include "b-string.h"

#ifdef SERVER
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif
#include "../misc/pagesize.h"

#include "../power-encoding/power-encoding.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   class BString;
   
   class Data
   {
   protected:
   public:
      Byte* _readWrite = nullptr;
      const Byte* _data;
      size_t _size = 0;
      bool _delete = false;
   public:
      typedef Byte ValueType;

      Data() : _readWrite(nullptr), _data(nullptr), _size(0)
      {
      }

      static Data create(size_t size = 0) {

         Data data;

         if (size == 0)
            size = getPageSize();

         data._readWrite = new Byte[size];
         data._delete = true;
         data._data = data._readWrite;
         data._size = size;
         
         memset(data._readWrite, 0, size);
         
         return data;
      }

      virtual ~Data() {
         if (_readWrite && _delete)
            delete[] _readWrite;
      }
      

      template<typename T>
      Data(T& source)
      {
         _readWrite = (Byte*)&source;
         _data = _readWrite;
         _size = sizeof(T);
         _delete = false;
      }
      /*
      Data(unsigned long& source)
      {
         _readWrite = (Byte*)&source;
         _data = _readWrite;
         _size = sizeof(unsigned long);
         _delete = false;
      }
      */
      template<typename T>
      Data(const vector<T>& source)
      {
         _delete = false;
         _readWrite = nullptr;
         _data = source.data();
         _size = source.size() * sizeof(T);
      }
/*      
      Data(const vector<Byte>& source) {
         _delete = false;
         _readWrite = nullptr;
         _data = source.data();
         _size = source.size();
      }
*/
      Data(const void* source, size_t len, bool copy = false) : Data((const Byte*)source, len, copy)
      {
      }
      
      Data(const char* source, size_t len, bool copy = false) : Data((const Byte*)source, len, copy)
      {
      }

      Data(const Byte* source, size_t len, bool copy = false)
      {
         if (copy) {
            _readWrite = new Byte[len];
            _delete = true;
            _data = _readWrite;
            _size = len;
            memcpy(_readWrite, source, _size);
         }
         else {
            _readWrite = nullptr;
            _data = source;
            _size = len;
            _delete = false;
         }
      }


      Data(const char* source) : Data(source, strlen(source)) {
         
      }
      
      Data(std::string string, bool copy = false) : Data(string.data(), string.size(), copy) {
         
      }

      // Implemented in misc.h
      Data(const BString& source);
      operator BString() const;
      
      Data(const Data& source, bool copy = false)
      {
         _size = source._size;
         if (copy)
         {
            _readWrite = new Byte[_size];
            _delete = true;
            _data = _readWrite;
            memcpy(_readWrite, source._data, _size);
         }
         else {
            _readWrite = nullptr;
            _data = source._data;
            _delete = false;
         }
      }

      template<typename T>
      Data& operator = (const T& rhs) {

         if (_readWrite) {
            if (_delete)
               delete[] _readWrite;
            _readWrite = nullptr;
            _delete = false;
         }

         _size = sizeof(T);
         _readWrite = new Byte[_size];
         memcpy(_readWrite, &rhs, _size);
         _data = _readWrite;
         _delete = true;

         return *this;
         
      }

      Data& operator = (const Data& rhs) {

         if (_readWrite) {
            if (_delete)
               delete[] _readWrite;
            _readWrite = nullptr;
            _delete = false;
         }
         _size = rhs._size;

         if (rhs._readWrite) {
            _readWrite = new Byte[rhs._size];
            _delete = true;
            memcpy(_readWrite, rhs._data, rhs._size);
            _data = _readWrite;
         }
         else {
            _readWrite = nullptr;
            _data = rhs._data;
         }
            
         return *this;
         
      }

      Data copy() {
         Data copyOfData = Data::create(_size);
         memcpy(copyOfData.data(), _data, _size);
         return copyOfData;
      }

      const char* data() const {
         return (const char*)_data;
      }

      char* data() {
         return (char*)_readWrite;
      }

      const char* c_str() const {
         return (const char*) _data;
      }

      virtual size_t size() const {
         return _size;
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

      void clear() {
         
      }
      
      //operator BString() const;

      BString toHex() const;
      

      // defined in base64.h
      // included from string.h
      BString toBase64() const;

      // defined in base64.h
      // included from string.h
      static Data fromBase64
      (const BString& data);

#ifdef SERVER
      
      BString md5() const;

      // sha3_512
      BString sha3() const;
      

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
         
   };
   
   
   
}

#endif
