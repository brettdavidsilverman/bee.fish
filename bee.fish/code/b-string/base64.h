#ifndef BEE_FISH__BASE64
#define BEE_FISH__BASE64

#include <string>
#include <vector>
#include "data.h"

using namespace BeeFishBString;

namespace BeeFishBase64
{

   // Lookup table for encoding
   // If you want to use an alternate alphabet,
   // change the characters here
   const char encodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const char padCharacter = '=';

   inline BString
   encode(const Byte* buffer, size_t size) {

      std::string encodedString;
      encodedString.reserve(((size/3) + (size % 3 > 0)) * 4);
      long temp;
      const Byte* cursor = &(buffer[0]);
      for(size_t idx = 0; idx < size/3; idx++)
      {
         temp  = (*cursor++) << 16; //Convert to big endian
         temp += (*cursor++) << 8;
         temp += (*cursor++);
         encodedString.push_back(encodeLookup[(temp & 0x00FC0000) >> 18]);
         encodedString.push_back(encodeLookup[(temp & 0x0003F000) >> 12]);
         encodedString.push_back(encodeLookup[(temp & 0x00000FC0) >> 6 ]);
         encodedString.push_back(encodeLookup[(temp & 0x0000003F)      ]);
      }
      
      switch(size % 3)
      {
      case 1:
         temp  = (*cursor++) << 16; //Convert to big endian
         encodedString.push_back(encodeLookup[(temp & 0x00FC0000) >> 18]);
         encodedString.push_back(encodeLookup[(temp & 0x0003F000) >> 12]);
         encodedString.push_back(padCharacter);
         encodedString.push_back(padCharacter);
         break;
      case 2:
         temp  = (*cursor++) << 16; //Convert to big endian
         temp += (*cursor++) << 8;
         encodedString.push_back(encodeLookup[(temp & 0x00FC0000) >> 18]);
         encodedString.push_back(encodeLookup[(temp & 0x0003F000) >> 12]);
         encodedString.push_back(encodeLookup[(temp & 0x00000FC0) >> 6 ]);
         encodedString.push_back(padCharacter);
         break;
      }
      return encodedString;
   }
   
   inline BString
   encode(const Data& data) {
      return encode((const Byte*)data.data(), data.size());
   }

   
   inline std::vector<Byte> decode(
      const BString& input
   )
   {


      if (input.size() % 4) //Sanity check
         throw std::runtime_error("Non-Valid base64!");
   
      size_t padding = 0;
      if (input.size())
      {
         if (input[input.size()-1] == padCharacter)
            padding++;
         if (input[input.size()-2] == padCharacter)
            padding++;
      }
  
      //Setup a vector to hold the result
      std::vector<Byte> decodedBytes;
      decodedBytes.reserve(((input.size()/4)*3) - padding);
      long temp=0; //Holds decoded quanta
      BStringBase::const_iterator cursor = input.begin();
   
      while (cursor < input.end())
      {
         for (size_t quantumPosition = 0; quantumPosition < 4; quantumPosition++)
         {
            temp <<= 6;
            auto value = *cursor;
            if (value >= 0x41 && value <= 0x5A) // This area will need tweaking if
               temp |= value - 0x41;                    // you are using an alternate alphabet
            else if  (value >= 0x61 && value <= 0x7A)
               temp |= value - 0x47;
            else if  (value >= 0x30 && value <= 0x39)
               temp |= value + 0x04;
            else if  (value == 0x2B)
               temp |= 0x3E; //change to 0x2D for URL alphabet
            else if  (value == 0x2F)
               temp |= 0x3F; //change to 0x5F for URL alphabet
            else if  (value == padCharacter) //pad
            {
               switch( input.end() - cursor )
               {
               case 1: //One pad character
                  decodedBytes.push_back((temp >> 16) & 0x000000FF);
                  decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
                  return decodedBytes;
               case 2: //Two pad characters
                  decodedBytes.push_back((temp >> 10) & 0x000000FF);
                  return decodedBytes;
               default:
                  throw std::runtime_error("Invalid Padding in Base 64!");
               }
            }  else
               throw std::runtime_error("Non-Valid Character in Base 64!");
            cursor++;
         }
         
         decodedBytes.push_back((temp >> 16) & 0x000000FF);
         decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
         decodedBytes.push_back((temp      ) & 0x000000FF);
      }
      return decodedBytes;
   }
   
}

namespace BeeFishBString
{
   inline BString Data::toBase64() const
   {
      return BeeFishBase64::encode(*this);
   }

   inline Data Data::fromBase64
   (const BString& base64)
   {
      std::vector<Byte> bytes =
          BeeFishBase64::decode(base64);
      Data data(bytes.data(), bytes.size(), true);
      return data;
   }
}


#endif