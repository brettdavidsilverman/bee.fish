#ifndef BEE_FISH_B_STRING__MISC_H
#define BEE_FISH_B_STRING__MISC_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#ifdef SERVER
#include <openssl/md5.h>
#endif

#include "../power-encoding/power-encoding.h"
#include "b-string.h"
#include "character.h"
#include "data.h"
#include "base64.h"
#include "bit-stream.h"

namespace BeeFishBString
{
   
   inline PowerEncoding& operator <<
   ( 
      PowerEncoding& stream,
      const char* string
   )
   {
      return operator <<
      (stream, BString(string));
   }

   
#ifdef SERVER
   inline BString Data::md5() const
   {
      
      Byte result[MD5_DIGEST_LENGTH];
      memset(result, 0, MD5_DIGEST_LENGTH);
         
      MD5(
         _data,
         size(),
         result
      );


      Data digest(result, MD5_DIGEST_LENGTH);

      return digest.toHex();
         
   }
   
   inline BString Data::sha3() const
   {
      uint32_t digest_length = SHA512_DIGEST_LENGTH;
      const EVP_MD* algorithm = EVP_sha3_512();
      uint8_t* digest = static_cast<uint8_t*>(OPENSSL_malloc(digest_length));
      EVP_MD_CTX* context = EVP_MD_CTX_new();
      EVP_DigestInit_ex(context, algorithm, nullptr);
      EVP_DigestUpdate(context, c_str(), size());
      EVP_DigestFinal_ex(context, digest, &digest_length);
      EVP_MD_CTX_destroy(context);
      Data dataDigest(digest, SHA512_DIGEST_LENGTH);
      BString output = dataDigest.toHex();
      OPENSSL_free(digest);
      return output;
   }
#endif

   inline BString Data::toHex() const
   {
      static const char digits[] = "0123456789abcdef";

      BString hex;

      for (size_t i = 0; i < size(); ++i) {
         hex += digits[_data[i] / 16];
         hex += digits[_data[i] % 16];
      }

      return hex;
      
   }

/*  
   // BString from data
   inline BString::BString(const Data &source)
   {
      size_t characterCount = source.size() / sizeof(Character);
      reserve(characterCount);
      Character* chars = (Character*)(source.data());
      for (size_t i = 0; i < characterCount; ++i) {
         BStringBase::push_back(chars[i]);
      }
   }
*/
/*
   // Declared in character.h
   inline BString Character::bstr() const {
      BString string;
      for (auto bit : (*this)) {
         if (bit)
            string.push_back('1');
         else
            string.push_back('0');
      }
      return string;
   }
*/
   // Data from BString
   inline Data::Data(const BString& source) : Data::Data(source.toData())
   {
   }

   inline Data::operator BString() const {
      return BString::fromData(*this);
   }

   inline Data BString::toData() const
   {
      BitStream stream;

      stream << *this;

      Data data = stream.toData();

      return data;
   }

   inline BString BString::fromData(const Data &source)
   {

      BitStream stream = BitStream::fromData(source);
      
      BString bstring;
      stream >> bstring;

      return bstring;
   }
}

#endif


