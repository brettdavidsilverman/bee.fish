#ifndef BEE_FISH_PARSER__PARSER_HPP
#define BEE_FISH_PARSER__PARSER_HPP

#include <iostream>
#include <string>
#include <bitset>

#include "ParserBase.hpp"

namespace BeeFishParser {

   class UTF8Character;

   class Parser : public ParserBase
   {
   public:
  
      
      virtual bool read(
         const std::string& string
      )
      {

         for (const char& c : string)
         {
            if (!read(c))
               return false;
         }

         return true;

      }

      virtual bool read(
         const char* stream
      )
      {
         const std::string string(stream);
         return read(string);
      }

      virtual bool read(
         char character
      )
      {

         std::bitset<8> bits = character;
         for (int i = 7;
              i >= 0;
              --i)
         {

            bool bit = bits[i];
            bool result =
               read(bit);

            if (result == false)
               return false;
         
         }

         capture(character);

         return true;
      }
      
      virtual bool read(bool bit) = 0;

      virtual bool read(
         const UTF8Character& utf8
      )
      {
         return true;
      }
      
      virtual const std::string& value() const
      {
         return EmptyString();
      }

      virtual void capture(char character)
      {
      }

      virtual Parser* copy() const = 0;
      


   };



}

#endif


