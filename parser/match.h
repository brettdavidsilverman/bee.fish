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
   typedef BeeFishBString::Character Char;

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
      

      bool _setup = false;
      BeeFishMisc::optional<bool> _result = BeeFishMisc::nullopt;
      Char _character = 0;
      Match* _match = nullptr;
      Parser* _parser = nullptr;
      
      typedef std::function<void(Match*)> OnSuccess;
      OnSuccess _onsuccess;

   public:
   
      Match()
      {
         _match = nullptr;
         ++matchInstanceCount();
      }

      Match(Match* match) : _match(match) {
         ++matchInstanceCount();
         _parser = _match->_parser;
      }

      Match(const Match& match) :
         _setup(false),
         _result(BeeFishMisc::nullopt),
         _character(0),
         _match(nullptr),
         _parser(match._parser),
         _onsuccess(match._onsuccess)
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
         return (result() == true);
      }
      

   public:

      virtual void setup(Parser* parser) {

         _parser = parser;

         if (_parser == nullptr)
            throw std::logic_error("Match::setup _parser is not defined.");
         
         if (_setup)
            return;
            
         _setup = true;

      }


      virtual bool match(
         Parser* parser,
         const Char& character
      )
      {
         if (!_setup)
            setup(parser);

         _character = character;


         bool matched = matchCharacter(character);

         if (matched) 
            capture();

         if (_result == true)
            success();
         else if (_result == false)
            fail();

         return matched;
      }
      
      virtual bool matchCharacter(const Char& character)  {
         
         if (!_setup)
            throw std::logic_error("Match::matchCharacter not setup");

         bool matched = false;

         if (!_match) {
            matched = true;
         }
         else  {
            matched = _match->match(_parser, character);
            _result = _match->_result;
         }

         return matched;
      };
      
      virtual void success()
      {
         if (_onsuccess)
            _onsuccess(this);
      }
   
      virtual void fail()
      {
      }
            
      virtual BeeFishMisc::optional<bool> result() const
      {
         return _result;
      }

      virtual BString value()
      {
         return EmptyString();
      }
      
      virtual const Char& character() const {
         return _character;
      }

      virtual void capture()
      {
      }
      
   
   };



}

#endif


