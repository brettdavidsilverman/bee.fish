#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../misc/debug.h"
#include <sstream>
#include <ostream>
#include <chrono>
#include <optional>

#include "version.h"
#include "misc.h"
#include "match.h"
#include "capture.h"
#include "character.h"
#include "range.h"

#include "word.h"
#include "ciword.h"
#include "and.h"
#include "or.h"
#include "not.h"
#include "optional.h"
#include "repeat.h"
#include "invoke.h"
#include "blanks.h"
#include "load-on-demand.h"



using namespace BeeFishBString;

namespace BeeFishParser
{
   

   class Parser
   {
   public:
      optional<bool> _result = nullopt;
   protected:
      
      Match* _match;
      size_t _charCount = 0;
      ssize_t _dataBytes = -1;
      char _lastCharacter = -1;
      BeeFishBString::UTF8Character _utf8 = "";

   public:
      Parser(Match& match) :
         _match(&match)
      {
      }
      
      Parser(Match* match) :
         _match(match)
      {
      }

      void setMatch(Match& match) {
         _match = &match;
         _utf8.reset();
         _charCount = 0;
      }      

      virtual ~Parser()
      {

      }
      
      Match* getMatch() {
         return _match;
      }

      unsigned long now()
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

      virtual bool match(uint8_t byte) {

         
#ifdef DEBUG
         cerr << (char)byte;
#endif
         ++_charCount;

         _lastCharacter = (char)byte;
         
         if (_dataBytes >= 0)
         {
            --_dataBytes;
            _match->match(this, byte);
         }
         else  {

            if (_utf8.match(byte))
            {
               _match->capture(this, byte);
            }
            
            if (_utf8.result() == true) {
               // Valid utf8 character, perform match
               _match->match(this, _utf8.character());
               // Reset the utf8 character
               _utf8.reset();
            }
            else if (_utf8.result() == false) {
               // in valid utf8 character, try to perform match
               _match->match(this, _utf8.character());
               // Reset the utf8 character
               _utf8.reset();
            }

         }         

         
         _result = _match->result();
         
         bool matched = _result != false;
            
         return matched;
 
      }
      
      virtual optional<bool> read(
         istream& input
      )
      {
       
#ifdef TIME
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         _result = nullopt;
         _match->_parser = this;

         int i = 0;
         while ((i = input.get()) != -1)
         {
            
            uint8_t c = i;

            if (!match(c))
               return false;

#ifdef TIME
            if (_charCount % 100000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout 
                  << _charCount << " (char count)\t"
                  << Match::matchInstanceCount() << " (instances)\t" 
                  << time << " (milliseconds)\t"
                  << _charCount / time * 100 << " (chars per second)" 
                  << endl;

               start = now();
            }
#endif
            if (_result != nullopt) {
               break;            
            }
         }

         return _result;
      }
   
      virtual optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      virtual optional<bool> read(const char* str) {
         return read(std::string(str));
      }

      virtual optional<bool> read(const BeeFishBString::Data& data, size_t size = 0)
      {

         if (size == 0)
            size = data.size();

         const Byte* _data = data._data;

         for ( size_t i = 0;
               i < size && _result == nullopt;
               ++i )
         {
            uint8_t byte = _data[i];
            if (!match(byte))
               return false;
         }

         return _result;
      
      }

      virtual optional<bool> read(const BeeFishBString::BString& string)
      {

         size_t _size = string.size();

         for (size_t i = 0; i < _size; ++i) {
            char character = string[i];
            if (!match(character))
               return false;
         }

         return _result;
      
      }
      
      virtual optional<bool> read(const char c)
      {

         if (!match(c))
            return false;

         return _result;
      
      }
      
      optional<bool> result() const
      {
         return _result;
      }

      virtual bool isJSONParser() {
         return false;
      }

      virtual bool isBScriptParser() {
         return false;
      }

      void setDataBytes(ssize_t dataBytes) {
         _dataBytes = dataBytes;
      }
      
      virtual void eof() {
         if (result() == nullopt && _match) {
            _match->eof(this);
         }
      }

      virtual string getErrorMessage() const {
         stringstream stream;
         stream << "Invalid Content '" << escape(_lastCharacter) << "' at position "
                << _charCount;
         return stream.str();
      }
   };
}

#endif