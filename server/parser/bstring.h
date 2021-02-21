#ifndef BEE_FISH_PARSER__BSTRING_H
#define BEE_FISH_PARSER__BSTRING_H

#include <iostream>
#include <string>
#include "match.h"
#include "repeat.h"
#include "utf-8.h"

namespace bee::fish::parser {

   class bstring :
      protected Repeat,
      public vector<Char>
   
   {
   public:
      // empty string
      bstring() : Repeat(UTF8Character())
      {
      }
      
      // utf-8 string 
      bstring(const std::string& utf8string) :
         Repeat(UTF8Character())
      {
         if (!read(utf8string))
            throw runtime_error("Invalid utf-8 string");
      }
      
      // utf-8 character
      bstring(const Char& utf8Character) :
         Repeat(UTF8Character())
      {
         if (!match(utf8Character))
            throw runtime_error("Invalid utf-8 string");
      }
   
      virtual void matchedItem(Match* item)
      {
         UTF8Character* utf8 =
            (UTF8Character*)item;
         
         push_back(
            utf8->character()
         );
      
         Repeat::matchedItem(item);
      }
      
      static void write(ostream& out, const bstring& bstr)
      {
         for (auto c : bstr)
         {
            writeCharacter(out, c);
         }
      }
      
      static void writeCharacter(ostream& out, const Char& character)
      {
         switch (character)
         {
         case '\"':
            out << "\\\"";
            break;
         case '\\':
            out << "\\\\";
            break;
         case '\b':
            out << "\\b";
            break;
         case '\f':
            out << "\\f";
            break;
         case '\r':
            out << "\\r";
            break;
         case '\n':
            out << "\\n";
            break;
         case '\t':
            out << "\\t";
            break;
         case Match::EndOfFile:
            out << "-1";
            break;
         default:
            writeUTF8Character(out, character);
         }
      }
     
      static void writeUTF8Character(
         ostream& out,
         const Char& character
      )
      {
         if (character <= 0x007F)
         {
            char c1 = (char)character;
            out << c1;
         }
         else if (character <= 0x07FF)
         {
            //110xxxxx 10xxxxxx
            char c1 = ( 0b00011111         &
                      ( character >> 6 ) ) |
                        0b11000000;
                            
            char c2 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2;
         }
         else if (character <= 0xFFFF)
         {
            //1110xxxx 10xxxxxx 10xxxxxx
            char c1 = ( 0b00001111          &
                      ( character >> 12 ) ) |
                        0b11100000;
                           
            char c2 = ( 0b00111111          &
                      ( character >>  6 ) ) |
                        0b10000000;
                           
            char c3 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                           
            out << c1 << c2 << c3;

         }
         else if (character <= 0x10FFFF)
         {
            //11110xxx 10xxxxxx
            //10xxxxxx 10xxxxxx
            char c1 = ( 0b00000111         &
                      ( character >> 18) ) |
                        0b11110000;
                            
            char c2 = ( 0b00111111         &
                      ( character >> 12) ) |
                        0b10000000;
                            
            char c3 = ( 0b00111111         &
                      ( character >>  6) ) |
                        0b10000000;
                            
            char c4 = ( 0b00111111 &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2 << c3 << c4;
         }
         else
         {
            out << "{"
                << character
                << "}";
         }
   
      }
      
   };


   inline ostream& operator << (ostream& out, const bstring& bstr)
   {
      bstring::write(out, bstr);
      
      return out;
   }
   
   
}

#endif


