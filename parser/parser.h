#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../misc/debug.h"
#include "../misc/optional.h"
#include <sstream>
#include <ostream>
#include <chrono>


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
   protected:
      BeeFishMisc::optional<bool> _result = BeeFishMisc::nullopt;
      Match& _match;
      size_t _charCount = 0;
      signed long long _dataBytes = -1;

      BeeFishBString::UTF8Character _utf8 = -1;

   public:
      Parser(Match& match) :
         _match(match)
      {
      }

      void setMatch(Match& match) {
         _match = match;
         _utf8.reset();
         _charCount = 0;
      }      

      virtual ~Parser()
      {

      }
      
      Match* getMatch() {
         return &_match;
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

         
#ifdef DEBUG_PARSER
         cerr << (char)byte;
#endif
         ++_charCount;


         if (_dataBytes >= 0)
         {
            --_dataBytes;
            _match.match(this, byte);
         }
         else {

            _utf8.match(byte);

            if (_utf8.result() == true) {
               // Valid utf8 character, perform match
               _match.match(this, _utf8.character());
               // Reset the utf8 character
               _utf8.reset();
            }
            else if (_utf8.result() == false) {
               // in valid utf8 character, try to perform match
               _match.match(this, _utf8.character());
               // Reset the utf8 character
               _utf8.reset();
            }

         }         

         _result = _match._result;
         
         if (_result == BeeFishMisc::nullopt || _result == true)
            return true;

         return false;

      }
      
      virtual BeeFishMisc::optional<bool> read(
         istream& input
      )
      {
       
#ifdef TIME
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         _result = BeeFishMisc::nullopt;
         _match._parser = this;

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
                  << 10000.0 / time * 1000.0 << " (chars per second)" 
                  << endl;

               start = now();
            }
#endif
            if (_result != BeeFishMisc::nullopt) {
               break;            
            }
         }

         return _result;
      }
   
      virtual BeeFishMisc::optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      virtual BeeFishMisc::optional<bool> read(const char* str) {
         return read(std::string(str));
      }

      virtual BeeFishMisc::optional<bool> read(const BeeFishBString::Data& data, size_t size = 0)
      {

         if (size == 0)
            size = data.size();

         const Byte* _data = data._data;

         for ( size_t i = 0;
               i < size && _result == BeeFishMisc::nullopt;
               ++i )
         {
            uint8_t byte = _data[i];
            if (!match(byte))
               return false;
         }

         return _result;
      
      }

      virtual BeeFishMisc::optional<bool> read(const BeeFishBString::BString& string)
      {

         size_t _size = string.size();

         for (size_t i = 0; i < _size; ++i) {
            BeeFishBString::Character character = string[i];
            if (!match(character))
               return false;
         }

         return _result;
      
      }

      BeeFishMisc::optional<bool> result() const
      {
         return _result;
      }

      virtual bool isJSONParser() {
         return false;
      }

      virtual bool isBScriptParser() {
         return false;
      }

      void setDataBytes(signed long long dataBytes) {
         _dataBytes = dataBytes;
      }

   };
}

#endif