#ifndef BEE_FISH_PARSER__PARSER_H
#define BEE_FISH_PARSER__PARSER_H

#include <iostream>
#include <string>
#include <bitset>

#include "../Miscellaneous/Miscellaneous.hpp"
using namespace std;

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
      
      virtual Optional read(
         const UTF8Character& utf8
      )
      {
         return true;
      }

      virtual Optional read(
         const char* stream
      )
      {
         std::string string(stream);
         bool parsed = true;

         for (std::string::const_iterator it =
                 string.cbegin();
              it != string.cend();
              ++it
             )
         {
            parsed = read(*it);
            if (!parsed)
               break;
         }

         return parsed;

      }

      virtual Optional read(
         char character
      )
      {
         bitset<8> bits = character;
         for (int i = 0;
              i < 8;
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

      virtual Optional read(bool bit) {
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


