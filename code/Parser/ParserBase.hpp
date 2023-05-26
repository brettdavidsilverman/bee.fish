#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <bitset>
#include <memory>
#include "../Miscellaneous/optional.h"

namespace BeeFishParser {

   class UTF8Character;

   class ParserBase {
#ifdef DEBUG
   protected:
      std::string _value;
#endif

   public:
      std::optional<bool> _result = std::nullopt;

   public:
   
      ParserBase()
      {
         ++parserInstanceCount();
      }

      ParserBase(const ParserBase& parser)
      {
         ++parserInstanceCount();
      }
      
      virtual ~ParserBase()
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
         size_t count = 0;

         for (const char& c : string)
         {
           // if (_result == false)
          //     return false;

            if (!read(c))
               break;
            ++count;
#ifdef DEBUG
            _value.push_back(c);
#endif
         }

         if (count != string.length())
         {
            setResult(false);
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

      virtual ParserBase* copy() const = 0;
      
      virtual bool isOptional() const;
      
      virtual size_t index() {
         if (parsed())
            return 1;

         return 0;
      }

      virtual std::shared_ptr<ParserBase>
      operator [] (size_t index)
      {
         if (index == 0)
            return 
               std::shared_ptr<ParserBase>(this);

         return
            std::shared_ptr<ParserBase>(nullptr);
      }

   };

   typedef ParserBase Parser;
}

#endif
