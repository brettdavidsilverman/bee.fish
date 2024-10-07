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
   
   class CharacterBase : public Match
   {
   protected:
      Char _character;
      
   public:
      CharacterBase() : Match()
      {
      }
       
      virtual const Char& character() const
      override
      {
         return _character;
      }
   };

   class PlainCharacter : public CharacterBase
   {

   public:
      PlainCharacter() : CharacterBase() 
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
            
         setResult(_match->result());

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

   class Hex : public CharacterBase
   {
   private:
      Char _hexValue;

   public:
      Hex() : CharacterBase()
      {
          _match = new Capture(
             new Repeat<HexCharacter>(
                4, 4
             )
          );
      }
            
      virtual void success()
      {
         std::stringstream stream;
         stream << std::hex << value();
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
      public CharacterBase
      
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
                  success();
                  return true;
               }
            );
      }
      
      
   public:
      EscapedCharacter() :
         CharacterBase()
      {
          Match* invokeHex = new
            Invoke(
               _hex = new Hex(),
               [this](Match*)
               {
                  _character = 
                     _hex->character();
                  return true;
               }
            );
            
         _match = new And(
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
      }
      
      virtual ~EscapedCharacter() {
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
      override
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

      virtual void matchedItem(StringCharacter* item)
      override
      {
         const Char& character = item->character();
         //cerr << "{" << character << "}" << std::flush;
         BString::push_back(character);
         BStream::write(character);
         Repeat::matchedItem(item);
      }
      
      virtual void fail()
      override
      {
          //assert(false);
      }

      virtual BString& value() {
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
            [this](const BeeFishBString::Data& buffer) {
               _value = _stringCharacters->value();
               if (this->_onbuffer) {
                  this->_onbuffer(buffer);
               }
               
            };


         _match = new And(
            new Quote(),
            _stringCharacters,
            new Quote()
         );


         Match::setup(parser);
      }
      
      virtual BString& value()
      {
         return _value;
      }
      
      virtual const BString& value() const
      {
         return _value;
      }
      
      virtual void success() {
         _stringCharacters->flush();
      }
      
   protected:
      
   };


}

#endif