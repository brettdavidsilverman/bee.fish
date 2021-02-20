#ifndef BEE_FISH_PARSER_JSON__STRING_H
#define BEE_FISH_PARSER_JSON__STRING_H

#include <iomanip>
#include <iostream>       // std::cout, std::hex
#include <string>         // std::string, std::u16string
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8_utf16

#include "../parser.h"
#include "value.h"

namespace bee::fish::parser::json {

   class String :
      public And,
      public Value
   {
   protected:
      
   public:
      String(string str = "") : And(
         new Character('\"'),
         (Match*)(str.length() ?
            (Match*)(new Word(str)) :
            (Match*)(new Optional(
               new StringCharacters()
               )
            )
         ),
         new Character('\"')
      )
      {
      }
      
      virtual std::string name()
      {
         return "String";
      }
      
      class PlainCharacter : public Not
      {
      public:
         PlainCharacter() : Not(
            new Or(
               new Character('\\'),
               new Character('\"'),
               new Character('\r'),
               new Character('\n')
            )
         )
         {
            _capture = true;
         }
         
         char character()
         {
            return value()[0];
         }
      };
      
      class StringCharacter : public Or
      {
      public:
         class EscapedCharacter;
      protected:
         PlainCharacter* _plainCharacter;
         EscapedCharacter* _escapedCharacter;
      public:
         StringCharacter() : Or (
            _plainCharacter =
               new PlainCharacter(),
            _escapedCharacter =
               new EscapedCharacter()
         )
         {
         
            _capture = true;
            
         }
        
         class UnicodeHex : public And
         {
         protected:
         
         public:
            UnicodeHex() : And(
               new Character('u'),
               new And(
                  new Hex(),
                  new Hex(),
                  new Hex(),
                  new Hex()
               )
            )
            {
            }
            
            virtual string& hex()
            {
               return (*this)[1].value();
            }
            
            virtual string character()
            {
               const char *hexString = hex().c_str();
               unsigned int hexNumber;
               sscanf(hexString, "%x", &hexNumber);
               char high = (char)(hexNumber >> 8);
               char low = (char)(hexNumber & 0x00FF);
               string value;
               value += high;
               value += low;
               return value;
            }
            
            class Hex : public Or
            {
            public:
               Hex() : Or(
                  new Range('0', '9'),
                  new Range('a', 'f'),
                  new Range('A', 'F')
               )
               {
               }
            };
            
         };
         
         class EscapedCharacter : public And
         {

         protected:
            UnicodeHex* _unicodeHex;
            
         public:
            EscapedCharacter() : And(
               new Character('\\'),
               new Or(
                  new Character('\\'),
                  new Character('b'),
                  new Character('f'),
                  new Character('r'),
                  new Character('n'),
                  new Character('t'),
                  new Character('\"'),
                  _unicodeHex = new UnicodeHex()
               )
            )
            {
            }
            
            virtual string character() {
               switch (value()[1]) {
               case '\\':
                  return "";
               case 'b':
                  return "\b";
               case 'f':
                  return "\f";
               case 'r':
                  return "\r";
               case 'n':
                  return "\n";
               case 't':
                  return "\t";
               case '\"':
                  return "\"";
               case 'u':
                  return _unicodeHex->character();
               }
               
               return "\0";
            }
         };
         
         
         virtual string character()
         {
            if (_index == 0)
            {
               char c = _plainCharacter->character();
               string str;
               str += c;
               return str;
            }
            else
            {
               return
                  _escapedCharacter->
                     character();
            }
         }
         
      };
      
      class StringCharacters: public
         Repeat<StringCharacter>
        
      {
      protected:
         string _value;
         
      public:
      
         StringCharacters() : Repeat()
         {
            _capture = true;
         }
         
         
         virtual void addItem(Match* item)
         {
            if (_capture)
            {
               StringCharacter* character =
                  (StringCharacter*)item;
               string str = character->character();
               _value += str;
              // wcerr << _wvalue << endl;
            }
            Repeat::addItem(item);
         }
         
         virtual string& value()
         {
            return _value;
         }
         
      };
      
      virtual void write(ostream& out)
      {
         out << '\"';
         
         string& str = value();
         String::write(out, str);
         
         out << '\"';
      }
      /*
      static void write(ostream& out, const string& str)
      {
         for (auto it = str.begin();
                   it != str.end();
                   ++it )
                   
         {
           
             wchar_t wc = *it;
             long number = ( long)wc;
             short high = (number & 0xFF00) >> 8;
             short low = (number & 0x00FF);
            
            if (high)
            {
               out << "\\u"
                   << std::hex 
                   << std::uppercase 
                   << std::setw(2) 
                   << std::setfill('0') 
                   << high
                   << low;
            }
            else
            {
               Match::write(
                  out,
                  (unsigned int)low
               );
            }
            
         }
         
      }
      
      */
      static void write(wostream& wout, const wstring& wstr)
      {
         wout << wstr;
      }
      
      static void write(ostream& out, const string& str)
      {
         for (const char& c : str)
         {
            if (c >= 0x00 && c <= 0x1F)
            {

               out << "\\u00"
                   << std::uppercase 
                   << std::setw(2) 
                   << std::setfill('0') 
                   << std::right
                   << std::hex
                   << (unsigned char)c;
            }
            else
               Match::write(out, c);
         }
      }
      
      virtual StringCharacters& characters()
      {
         Optional& optional =
            (Optional&)(*this)[1];
            
         return (StringCharacters&)
            (optional.item());
      }
      
      virtual wstring& wvalue()
      {
         return characters().wvalue();
      }
      
      virtual string& value()
      {
         return characters().value();
      }


   };
   
}

#endif