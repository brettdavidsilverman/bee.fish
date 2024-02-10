#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <memory>
#include <chrono>
#include <cstring>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Size.hpp"

namespace BeeFishParser {

   using namespace std;
   using namespace BeeFishMisc;

   class UTF8Character;

   class Parser {
   protected:
      
      
   public:
      std::string _value = "";
      std::optional<bool> _result = std::nullopt;
      size_t _byteCount = 0;
      char _lastCharacter = 0;

   public:
   
      Parser()
      {
         ++parserInstanceCount();
      }

      Parser(const Parser& source) :
          _value(source._value)
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

      virtual bool eof()
      {
          
         if (_result == nullopt)
            return read((char)-1);
            
         return false;
      }

      virtual string getErrorMessage() const {
         stringstream stream;
         stream << "Invalid Content '" << escape(_lastCharacter) << "' at position "
                << _byteCount;
         return stream.str();
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
         }

         return true;

      }

      virtual bool read(
         const char* string,
         int length
      )
      {
         const char* end = string + length;
         for (const char* pc = string;
              pc != end;
              ++pc)
         {
            if (!read(*pc))
               return false;

         }
         return true;
      }

      virtual bool read(
         const char* string
      )
      {
         for (const char* pc = string;
              *pc != 0;
              ++pc)
         {
            if (!read(*pc))
               return false;

         }

         return true;

      }

      virtual bool read(
         char character
      )
      {
         _lastCharacter = character;
         ++_byteCount;
         return true;
      }

      virtual bool readIndirect(
          Parser& parser,
          char c
      ) {
          
         if (parser._result != nullopt)
            return false;

         bool matched = true;
         
         matched = matched &&
            Parser::read(c);
         

         matched = matched &&
            parser.read(c);

         if (parser._result != nullopt)
            setResult(parser._result);

         return matched;
      }
      
      friend std::istream& operator >>
      (std::istream& in, Parser& parser)
      {
         using namespace std;
         using namespace BeeFishMisc;

#ifdef TIME
         cout << "Chars" << "\t" << "Parsers" << "\t" << "Time" << endl;
         unsigned long start = now();
         unsigned long diff = start;
#endif
         size_t charCount = 0;
         int c = -1;
         while ((c = in.get()) > 0)
         {
            ++charCount;

#ifdef DEBUG
            cout << (char)c;
#endif

            if (!parser.read((char)c))
               break;

            if (parser._result != nullopt)
               break;

#ifdef TIME
            if (charCount % 100000 == 0)
            {
               unsigned long time =
                  now() - diff;
               
               cout 
                  << charCount << " (char count)\t"
                  << Parser::parserInstanceCount() << " (instances)\t" 
                  << time << " (milliseconds)\t"
                  << (100000.0 / time) << " (kb/sec)" 
                  << endl;

               diff = now();
            }
#endif

         }

#ifdef TIME
         unsigned long time =
            now() - start;
         cout 
            << charCount << " (char count)\t"
            << Parser::parserInstanceCount() << " (instances)\t" 
            << time << " (milliseconds)\t"
               << ((float)charCount / time) << " (kb/sec)" 
            << endl;
         
#endif

         return in;
      }

      bool readEndOfFile() {
         return read(-1);
      }

 
      friend std::istream& operator >>
      (std::istream& in, const Parser& parser)
      {
         std::unique_ptr<Parser>
            ptr(parser.copy());

         in >> *ptr;

         return in;
      }

      virtual bool read(
         const UTF8Character& utf8
      )
      {
         return true;
      }

      virtual const std::string& value() const
      {
         return _value;
      }
      
      virtual std::string& value()
      {
         return _value;
      }

      virtual bool flush() {
         return true;
      }

      virtual Parser* copy() const = 0;

      virtual bool isOptional() const {
         return false;
      };
      
      virtual Size index() {

         throw std::logic_error("Pure virtual function");
         
      }

      virtual Parser*
      getByIndex(Size index)
      {
         if (index == 0)
            return this;

         return nullptr;
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
