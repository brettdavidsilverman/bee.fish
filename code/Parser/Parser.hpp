#ifndef BEE_FISH_PARSER__PARSER_HPP
#define BEE_FISH_PARSER__PARSER_HPP

#include <iostream>
#include <string>
#include <bitset>

#include "ParserBase.hpp"

namespace BeeFishParser {

   typedef std::string String;
   typedef BeeFishMisc::optional<bool> Optional;

   class UTF8Character;

   class Parser : public ParserBase
   {
   protected:
      std::string _currentChars;
   public:
  
      
      virtual bool read(
         const std::string& string
      )
      {
         bool parsed = true;

         for (const char& c : string)
         {
            parsed = read(c);
            if (!parsed)
               break;
         }

         return parsed;

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
            Optional result =
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

      virtual void setup() {

         _result = NullOpt;

      }
      
      virtual const String& value() const
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


