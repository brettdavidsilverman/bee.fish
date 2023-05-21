#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON {

   class Quote : public BeeFishParser::Character {
   public:
      Quote() : Character('\"') {

      }
   };

   class PlainCharacter : public Match
   {

   public:
      PlainCharacter() : Match() 
      {
         _match = new Not(
            new Or(
               new BeeFishParser::
                  Character('\\'),
               new BeeFishParser::
                  Character('\"'),
               new BeeFishParser::
                  Character('\r'),
               new BeeFishParser::
                  Character('\n'),
               new BeeFishParser::
                  Character('\b'),
               new BeeFishParser::
                  Character('\f')
            )
         );
      }
      
      virtual bool matchCharacter(const Char& character)
      {
         bool matched = 
            _match->match(_parser, character);
            
         if (matched)
            _character = character;
            
         _result = _match->_result;

         return matched;
      }
      
   };
   
   class HexCharacter : public Or {
   public:
      HexCharacter() : Or (
         new Range('0', '9'),
         new Range('a', 'f'),
         new Range('A', 'F')
      )
      {

      }
   };

   class Hex : public Capture
   {
   private:
      BString _hex;
      Char _hexValue;

   public:
      Hex() : Capture(
         new Repeat<HexCharacter>(
            4, 4
         ),
         _hex
      )
      {
      }
            
      virtual void success()
      {
         std::stringstream stream;
         stream << std::hex << _hex;
         uint32_t u32;
         stream >> u32;
         _hexValue = Char(u32);

         Match::success();

      }

      virtual const Char& character() const {
         return _hexValue;
      }
          
   };
      
   class EscapedCharacter :
      public Match
      
   {
   private:
      Hex* _hex = nullptr;
      
   protected:

      Match* captureCharacter(
         Char input,
         Char output
      )
      {
         return
            new Invoke(
               new BeeFishParser::
                  Character(input),
               [this, output](Match*)
               {
                  _character = output;
               }
            );
      }
      
      
   public:
      EscapedCharacter() :
         Match()
      {
      }
      
      virtual ~EscapedCharacter() {
      }

      virtual void setup(Parser* parser)
      {
         
         Match* invokeHex = new
            Invoke(
               _hex = new Hex(),
               [this](Match*)
               {
                  _character = 
                     _hex->character();
               }
            );
            
         Match* match = new And(
            new BeeFishParser::
               Character('\\'),
            new Or(
               captureCharacter('\\', '\\'),
               captureCharacter('b', '\b'),
               captureCharacter('f', '\f'),
               captureCharacter('r', '\r'),
               captureCharacter('n', '\n'),
               captureCharacter('t', '\t'),
               captureCharacter('\"', '\"'),
               new And(
                  new PlainCharacter(),
                  new Not(new Hex())
               ),
               new And(
                  new BeeFishParser::
                     Character('u'),
                  invokeHex
               )
            )
         );
         
         _match = match;

         Match::setup(parser);
         
      }
      
      virtual const Char& character() const
      {
         return _character;
      }

      
   };
    
   class StringCharacter :
      public Match
   {
   protected:
   
      Or* _items = new Or(
         new PlainCharacter(),
         new EscapedCharacter()
      );
      
      
   public:
      StringCharacter()
      {
         _match = _items;
      }
      
      virtual const Char& character() const
      {
         return _items->_item->character();
      }
      
   };
         
   class StringCharacters :
      public Repeat<StringCharacter>,
      public BeeFishBString::BStream,
      public BString
   {

   public:
      StringCharacters() : Repeat(0,0)
      {
      }

      virtual void matchedItem(StringCharacter* item) {
         BeeFishBString::Character character = item->character();
         BString::push_back(character);
         *this << character;
         Repeat::matchedItem(item);
      }

      virtual BString value() {
         return *this;
      }

      virtual void clear() {
         BString::clear();
         BeeFishBString::BStream::clear();
      }

   };

   class String :
      public Match
   {
   public:
      BStream::OnBuffer _onbuffer;
   protected:
      StringCharacters*
         _stringCharacters = nullptr;
      BString _value;

   public:
      String() : Match()
      {
      }
      
      virtual void setup(Parser* parser)
      {
            
         _stringCharacters =
            new StringCharacters();
         
         _stringCharacters->_onbuffer =
            [this](const Data& buffer) {
               if (this->_onbuffer) {
                  this->_onbuffer(buffer);
               }
               this->_value += this->_stringCharacters->value();
               _stringCharacters->clear();
            };


         _match = new And(
            new Quote(),
            _stringCharacters,
            new Quote()
         );


         Match::setup(parser);
      }
      
      virtual BString value()
      {
         return _value;;
      }

      virtual void success() {
         _stringCharacters->flush();
      }
      
   protected:
      
   };


}

#endif