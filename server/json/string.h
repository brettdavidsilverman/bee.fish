#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iomanip>
#include <iostream>       // std::cout, std::hex
#include <string>         // std::string, std::u16string
#include <locale>         // std::wstring_convert
#include <codecvt>        // std::codecvt_utf8_utf16

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json {

   class String :
      public And
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
         
         Char character()
         {
            return _value[0];
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
            And* _hex;
            
         public:
            UnicodeHex() : And(
               new Character('u'),
               _hex = new And(
                  new Hex(),
                  new Hex(),
                  new Hex(),
                  new Hex()
               )
            )
            {
               _hex->_capture = true;
            }
            
            virtual BString& hex()
            {
               return _hex->_value;
            }
            
            virtual string character()
            {
               /*
               const char *hexString = hex().c_str();
               unsigned int hexNumber;
               sscanf(hexString, "%x", &hexNumber);
               char high = (char)(hexNumber >> 8);
               char low = (char)(hexNumber & 0x00FF);
               string value;
               value += high;
               value += low;
               return value;
               */
               throw runtime_error("Not implemented");
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
               switch (_value[1]) {
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
         Repeat
        
      {
      public:
         BString _value;
         
      public:
      
         StringCharacters() :
            Repeat(new StringCharacter())
         {
         }
         
         
         virtual void matchedItem(Match* item)
         {
            if (_capture)
            {
               StringCharacter* character =
                  (StringCharacter*)item;
               string str = character->character();
               _value += str;
              // wcerr << _wvalue << endl;
            }
            Repeat::matchedItem(item);
         }
         
      };
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex);
         out << '\"';
         
         const BString& str = _value;
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
      
      static void write(ostream& out, const BString& str)
      {
         for (const Char& c : str)
         {
            if (c >= 0x00 && c <= 0x1F)
            {

               out << "\\u00"
                   << std::uppercase 
                   << std::setw(2) 
                   << std::setfill('0') 
                   << std::right
                   << std::hex
                   << c;
            }
            else
               UTF8Character::writeEscaped(out, c);
         }
      }
      
      virtual StringCharacters& characters()
      {
         Optional& optional =
            (Optional&)(*this)[1];
            
         return (StringCharacters&)
            (optional.item());
      }
      
      virtual BString& value()
      {
         return characters()._value;
      }


   };
   
}

#endif