#ifndef BEE_FISH_PARSER__PARSER_H
#define BEE_FISH_PARSER__PARSER_H

#include <iostream>
#include <string>
#include <bitset>

#include "../Miscellaneous/Miscellaneous.hpp"

namespace BeeFishParser {

   typedef std::string String;
   typedef BeeFishMisc::optional<bool> Optional;
   #define NullOpt BeeFishMisc::nullopt

   class UTF8Character;

   class Parser {
   public:
      
      Optional _result = NullOpt;

   public:
   
      Parser()
      {
         ++parserInstanceCount();
      }

      Parser(const Parser& parser)
      {
         ++parserInstanceCount();
      }
      
      virtual ~Parser()
      {
         --parserInstanceCount();
      }
      
   public:
  
      bool parsed() const
      {
         return (result() == true);
      }
      
      virtual Optional result() const
      {
         return _result;
      }

      virtual void setResult(Optional result) {

         _result = result;

         if (result == true)
            success();
         else if (result == false)
            fail();

      }

      virtual String value()
      {
         return EmptyString();
      }
      
      
      virtual bool read(
         const char* stream, size_t length
      )
      {
         bool parsed = true;

         const char* cend = stream + length;

         for (const char* c = stream;
              c != cend;
              ++c)
         {
            parsed = read(*c);
            if (!parsed)
               break;
         }

         return parsed;

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

      virtual bool read(bool bit) {
         return true;
      }


      virtual bool read(
         const UTF8Character& utf8
      )
      {
         throw 1;
         return true;
      }

      virtual void setup() {

         _result = NullOpt;

      }

   protected:

      
      
      virtual void success()
      {
      }
   
      virtual void fail()
      {
      }

      virtual void capture(char character)
      {
      }

      static const String& EmptyString() {
         static const String _EmptyString = {};
         return _EmptyString;
      }

      static unsigned long& parserInstanceCount() {
         static unsigned long _parserInstanceCount = 0;
         return _parserInstanceCount;
      }
   };



}

#endif


