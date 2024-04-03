#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <bitset>
#include "../b-string/string.h"
#include "../misc/optional.h"

#include "misc.h"

using namespace std;

namespace BeeFishParser {

   class Parser;

   class Match {
   public:

      static const BString& EmptyString() {
         static const BString _EmptyString = {};
         return _EmptyString;
      }

      static unsigned long& matchInstanceCount() {
         static unsigned long _matchInstanceCount = 0;
         return _matchInstanceCount;
      }
      

      //bool _setup = false;
      optional<bool> _result = nullopt;
      Char _character = "";
      Match* _match = nullptr;
      Parser* _parser = nullptr;
      
      //typedef std::function<void(Match*)> OnSuccess;
      //OnSuccess _onsuccess;

   public:
   
      Match(void* params = nullptr)
      {
         _match = nullptr;
         ++matchInstanceCount();
      }

      Match(Match* match) : _match(match) {
         ++matchInstanceCount();
         _parser = _match->_parser;
      }

      Match(const Match& match) :
         _result(nullopt),
         _character(""),
         _match(nullptr),
         _parser(match._parser)
        // _onsuccess(match._onsuccess)
      {
         ++matchInstanceCount();
      }
      
      virtual ~Match()
      {
         if (_match)
            delete _match;

         --matchInstanceCount();
      }
      
   public:
  
      bool matched() const
      {
         return (result(true));
      }
      

   public:

      virtual void setup(Parser* parser) {

         if (_parser)
            return;
            
         _parser = parser;

         if (_parser == nullptr)
            throw std::logic_error("Match::setup _parser is not defined.");
         
         if (_match)
            _match->setup(parser);
            

      }


      virtual bool match(
         Parser* parser,
         const Char& character
      )
      {
         if (!_parser)
            setup(parser);

         _character = character;


         bool matched = matchCharacter(character);

         if (_result == true)
            success();
         else if (_result == false)
            fail();

         return matched;
      }
      
      virtual bool matchCharacter(const Char& character)  {
         
         if (!_parser)
            throw std::logic_error("Match::matchCharacter not setup");

         bool matched = false;

         if (!_match) {
            matched = true;
         }
         else  {
            matched = _match->match(_parser, character);
            _result = _match->result();
         }

         return matched;
      };
      
      virtual void success()
      {
         _result = true;
         //if (_onsuccess)
         //   _onsuccess(this);
      }
   
      virtual void fail()
      {
         _result = false;
      }
            
      virtual const optional<bool>& result() const
      {
         if (_match)
            return _match->result();
            
         return _result;
      }

      virtual bool result(optional<bool> compare) const
      {
         return result() == compare;
      }
      
      virtual const BString& value() const
      {
         if (_match)
            return _match->value();
            
         static BString emptyString = "";
         return emptyString;
      }
      
      virtual const Char& character() const {
         return _character;
      }
      
      virtual void capture(Parser* parser, char c)
      {
         if (!_parser)
            setup(parser);
            
         if (_match)
         {
            _match->capture(parser, c);
         }
      }
      
      virtual void eof(Parser* parser) {
         if (!_parser)
            setup(parser);
            
         if (_match)
            _match->eof(parser);
      }
      
      virtual void write(ostream& out) const {
         out << this->value();
      }
      
      friend ostream& operator << (ostream& out, const Match& number);
     
      friend istream& operator >> (istream& in, Match& number);
     
   
   };



}

#endif


