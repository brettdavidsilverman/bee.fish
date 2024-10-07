#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../Miscellaneous/Debug.hpp"
#include <sstream>
#include <ostream>
#include <chrono>

#include "version.h"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../b-string/b-string.h"
#include "../Script/Variable.hpp"

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
using namespace BeeFishScript;

namespace BeeFishParser
{
   

   class Parser
   {
   public:
      optional<bool> _result;
   protected:
      
      Match* _match;
      size_t _charCount = 0;
      ssize_t _dataBytes = -1;
      BeeFishBString::UTF8Character _utf8 = "";
      String _error;
   public:
      Char _lastCharacter = "";
      
   protected:
             
      Parser() : _match(nullptr)
      {
      }
      
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

         if (!_match->_parser)
            _match->setup(this);
            
#ifdef DEBUG
         cout << escape((char)byte);
#endif
         ++_charCount;

         
         if (_dataBytes >= 0)
         {
            --_dataBytes;
            _match->match(this, byte);
         }
         else  {

            
            
            // valid or invalid, continue on
            if (_utf8.match(byte) &&
                _utf8.result() != nullopt)
            {
               _lastCharacter = _utf8.character();
               // utf8 character, perform match
               _match->match(this, _lastCharacter);
               // Reset the utf8 character
               _utf8.reset();
            }

         }         

         
         _result = _match->result();
         
         bool matched = (_result != false);
            
         return matched;
 
      }
      
      virtual optional<bool>
      read(
         istream& input
      )
      {
       
#ifdef TIME
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         _result = nullopt;
         
         if (!_match->_parser)
            _match->setup(this);

         int i = 0;
         uint8_t c;
         while ((i = input.get()) != -1)
         {
            
            c = i;
#ifdef DEBUG
            cerr << '{' << c << '}';
#endif
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
                  << 100000.0 / (float)time << " (k chars per second)" 
                  << endl;

               start = now();
            }
#endif
            if (_result != nullopt) {
               break;            
            }
         }
         
         if (result() == true)
            success();
         else if (result() == false)
            fail();

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
          
         if (_match && 
            _match->result() == nullopt)
         {
            _match->eof(this);
            _result = _match->result();
         }
         
         if (_result == false)
            fail();
         else if (_result == true)
            success();
         
      }

      virtual const String& getError() const {
         return _error;
      }
      
      virtual void success()
      {
         _match->setResult(true);
         _result = true;
         _error.clear();
      }
      
      virtual void fail() {
         stringstream stream;
         stream << "Invalid Content '" << escape(_lastCharacter) << "' at position "
                << _charCount;
         fail(stream.str());
      }
      
      virtual void fail(const String& error)
      {
         _match->setResult(false);
         _result = false;
 
          setError(error);
         
      }
      
      virtual void setError(const BString& error)
      {
         _error = error;
      }
   };
   
   ostream& operator << (ostream& out, const Match& match)
   {
      match.write(out);
      return out;
   }
     
   istream& operator >> (istream& in, Match& match)
   {
      Parser parser(match);
      parser.read(in);
      return in;
   }
   
   // Declared in match.h
   const Char& Match::character() const {
      return _parser->_lastCharacter;
   }
            
   // Declared in match.h
   void Match::fail(const BString& error)
   {
      _parser->setError(error);
  
      setResult(false);
   }
}

#endif