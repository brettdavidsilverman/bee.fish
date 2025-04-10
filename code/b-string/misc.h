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
   /*
   static std::string toBase64(const std::string &in) {

      std::string out;

      int val = 0, valb = -6;
      for (unsigned char c : in) {
         val = (val << 8) + c;
         valb += 8;
         while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
            valb -= 6;
         }
      }
      if (valb>-6)
         out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
   
      while (out.size()%4)
         out.push_back('=');
         
      return out;
   }

   static std::string fromBase64(const std::string &in) {

      std::string out;

      std::vector<int> T(256,-1);
      for (int i=0; i<64; i++)
         T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

      int val=0, valb=-8;
      for (unsigned char c : in) {
         if (T[c] == -1) break;
            val = (val << 6) + T[c];
         valb += 6;
         if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
         }
      }
      return out;
   }
*/

#ifdef SERVER
/*
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
*/

   inline BString md5(const std::string& data)
   {
  
      EVP_MD_CTX*   context = EVP_MD_CTX_new();
      const EVP_MD* md = EVP_md5();
      unsigned char md_value[EVP_MAX_MD_SIZE];
      unsigned int  md_len;
      BString        output;

      EVP_DigestInit_ex2(context, md, NULL);
      EVP_DigestUpdate(context, data.c_str(), data.length());
      EVP_DigestFinal_ex(context, md_value, &md_len);
      EVP_MD_CTX_free(context);

      output.resize(md_len * 2);
      for (unsigned int i = 0 ; i < md_len ; ++i)
         std::sprintf(&output[i * 2], "%02x", md_value[i]);
 
      return output;
    
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


