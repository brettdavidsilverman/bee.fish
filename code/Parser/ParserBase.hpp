#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <bitset>
#include "../Miscellaneous/optional.h"

namespace BeeFishParser {

   class UTF8Character;

   class Parser {
#ifdef DEBUG
   protected:
      std::string _value;
#endif

   public:
      std::optional<bool> _result = std::nullopt;

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

      bool parsed() const
      {
         return (result() == true);
      }
      
      virtual std::optional<bool> result() const
      {
         return _result;
      }

      virtual void setResult(std::optional<bool> result) {

         if (_result == result) 
            return;

         _result = result;

         if (_result == true)
            success();
         else if (_result == false)
            fail();

      }

      virtual void success()
      {
      }
   
      virtual void fail()
      {
      }

      static const std::string& EmptyString() {
         static const std::string _EmptyString = {};
         return _EmptyString;
      }

      static unsigned long& parserInstanceCount() {
         static unsigned long _parserInstanceCount = 0;
         return _parserInstanceCount;
      }

      virtual bool read(
         const std::string& string
      )
      {
         using namespace std;

         for (const char& c : string)
         {
            if (!read(c))
              return false;

#ifdef DEBUG
            _value.push_back(c);
cerr << c << flush;

#endif
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

            if (!read(bit))
               return false;
         
         }


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
#ifdef DEBUG
         return _value;
#else
         return EmptyString();
#endif
      }

      virtual Parser* copy() const = 0;
      
      virtual bool isOptional() const;
      

   };
}

#endif
