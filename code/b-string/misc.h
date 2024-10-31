#ifndef BEE_FISH_B_STRING__MISC_H
#define BEE_FISH_B_STRING__MISC_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#ifdef SERVER
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

#include "../power-encoding/power-encoding.h"
#include "b-string.h"
#include "char.h"
#include "base64.h"
#include "bit-stream.h"

namespace BeeFishMisc
{
   inline BString toHex(const std::string& data)
   {
      static const char digits[] = "0123456789abcdef";

      BString hex;
      
      const Byte* _data =
         (const Byte*)data.data();
      
      for (size_t i = 0; i < data.size(); ++i) {
         hex += digits[_data[i] / 16];
         hex += digits[_data[i] % 16];
      }

      return hex;
      
   }
   
   inline std::string fromHex(const BString& hex)
   {
      std::string result(hex.length() / 2, '\0');
      
      for (size_t i = 0; i < hex.length(); i += 2)
      {
         std::string byteString = hex.substr(i, 2);
         char byte = (char) strtol(byteString.c_str(), NULL, 16);
         result[i / 2] = byte;
      }

      return result;
   }

#ifdef SERVER
   inline BString md5(const std::string& data)
   {
      
      std::string result(MD5_DIGEST_LENGTH, '\0');
         
      MD5(
         (const Byte*)data.data(),
         data.size(),
         (Byte*)result.data()
      );
      
      return toHex(result);
         
   }
   
   inline BString sha3(const std::string& data)
   {
      uint32_t digest_length = SHA512_DIGEST_LENGTH;
      const EVP_MD* algorithm = EVP_sha3_512();
      uint8_t* digest = static_cast<uint8_t*>(OPENSSL_malloc(digest_length));
      EVP_MD_CTX* context = EVP_MD_CTX_new();
      EVP_DigestInit_ex(context, algorithm, nullptr);
      EVP_DigestUpdate(context, data.data(), data.size());
      EVP_DigestFinal_ex(context, digest, &digest_length);
      EVP_MD_CTX_destroy(context);
      std::string dataDigest((char *)digest, SHA512_DIGEST_LENGTH);
      BString output = toHex(dataDigest);
      OPENSSL_free(digest);
      return output;
   }
   
   inline static std::string createRandom(
      size_t byteCount
   )
   {
      std::string buffer(byteCount, '\0');

      int rc = RAND_bytes(
         (Byte*) buffer.data(),
         byteCount
      );
      
      if (rc != 1)
      {
         // RAND_bytes failed
         throw runtime_error("Random bytes failed");
         
      }
      
      return buffer;
   }
   
#endif

   

}

#endif


