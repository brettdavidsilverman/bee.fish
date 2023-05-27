#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <bitset>
#include <memory>
#include <chrono>
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

      friend std::istream& operator >>
      (std::istream& in, Parser& parser)
      {
         using namespace std;

#ifdef TIME
         cout << "Chars" << "\t" << "Parsers" << "\t" << "Time" << endl;
         unsigned long start = now();
         
#endif
         size_t charCount = 0;
         int c = -1;
         while ((c = in.get()) > 0)
         {
            ++charCount;

            if (!parser.read((char)c))
               break;
#ifdef TIME
            if (charCount % 100000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout 
                  << charCount << " (char count)\t"
                  << Parser::parserInstanceCount() << " (instances)\t" 
                  << time << " (milliseconds)\t"
                  << 10000.0 / time * 1000.0 << " (chars per second)" 
                  << endl;

               start = now();
            }
#endif

         }

         return in;
      }

      friend std::istream& operator >>
      (std::istream& in, const Parser& parser)
      {
         std::unique_ptr<Parser>
            ptr(parser.copy());

         in >> *ptr;

         return in;
      }

      virtual bool read(bool bit) {
         return true;
      }

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

      virtual Parser* copy() const
      {
         return new Parser(*this);
      }
      
      virtual bool isOptional() const;
      
      virtual size_t index() {

         if (parsed())
            return 1;

         return 0;
      }

      virtual std::shared_ptr<Parser>
      operator [] (size_t index)
      {
         if (index == 0)
            return 
               std::shared_ptr<Parser>(this);

         return
            std::shared_ptr<Parser>(nullptr);
      }
#ifdef TIME
      static unsigned long now()
      {
         return
            std::chrono::duration_cast
            <std::chrono::milliseconds>
            (
               std::chrono::system_clock
                  ::now()
                     .time_since_epoch()
            ).count();
      }
#endif
   };


}

#endif
