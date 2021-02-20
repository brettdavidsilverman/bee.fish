#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "../id/id.h"

using namespace std;

inline ostream& operator <<
(ostream& out, optional<bool> ok)
{
   if (ok == true)
      out << "1";
   else if (ok == false)
      out << "0";
   else
      out << "?";
         
   return out;
}

using namespace bee::fish::server;

namespace bee::fish::parser {

   typedef wchar_t WideChar;

   class Match
   {
   protected:

      wstring _wvalue;
      optional<bool> _result = nullopt;
   
   public:
      static const WideChar EndOfFile = -1;
      bool _capture = true;
   
      vector<Match*> _inputs;
   
      template<typename ...T>
      Match(T*... inputs) :
         _inputs{inputs...}
      {

      }
   
      virtual ~Match() {
      
         for (auto
                it = _inputs.cbegin();
                it != _inputs.cend();
              ++it)
         {
            Match* child = *it;
            if (child)
            {
               delete child;
            }
         }
      
      }
   
      Match(const Match& source)
      {
      
         for (auto it = source._inputs.begin();
                   it != source._inputs.end();
                 ++it)
         {
            Match* match = *it;
            if (match)
            {
               Match* copy = match->copy();
               _inputs.push_back(copy);
            }
         }
      
      }
   
      virtual Match* copy() const = 0;
   
      virtual bool match(WideChar character)
      {
      
         if ( _capture &&
              character != Match::EndOfFile 
            )
         {
            _wvalue += character;
         }
      
         return true;
      }

      virtual bool read(
         istream& in,
         bool last = true
      )
      {
      
         _result = nullopt;
     
         while (!in.eof())
         {
            int character = in.get();

            if (character == Match::EndOfFile)
               break;
#ifdef DEBUG
            Match::write(cerr, character);
#endif
            match((WideChar)character);
         
            if (_result != nullopt)
               break;
            
         }

         if ( _result == nullopt &&
              last &&
              in.eof()
            )
         {
#ifdef DEBUG
            Match::write(cerr, Match::EndOfFile);
#endif
            match(Match::EndOfFile);
         
            return _result == true;
         }
     
         return (_result != false);
      }
   
      virtual bool read(
         const string& str,
         bool last = true
      )
      {
      
         istringstream in(str);
      
         return read(in, last);
      
      }
   
      virtual optional<bool> result()
      {
         return _result;
      }
   
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }
 
      friend ostream& operator <<
      (ostream& out, Match& match) 
      {

         match.write(out);
   
         return out;

      }
   
      virtual void writeResult(ostream& out) 
      {
         out << "<" << result() << ">";
      }
   
      virtual void writeInputs(ostream& out)
      {
         for (auto it = _inputs.begin(); 
                   it != _inputs.end();
                 ++it)
         {
            Match* input = *it;
            out << *input;
            if (it + 1 != _inputs.end())
               out << ", ";
         }
      }

      virtual void write(ostream& out) = 0;
   
      virtual Match& operator[]
      (size_t index)
      {
         return *(_inputs[index]);
      }
   
      virtual wstring& str()
      {
         return _wvalue;
      }
   
      virtual bool isOptional()
      {
         return false;
      }
   
   public:

   
      static void write(
         ostream& out,
         const wstring& text
      )
      {
         for (WideChar c : text)
         {
            write(out, c);
         }
         
      }
   
      static void write(ostream& out, WideChar character)
      {
         switch (character) {
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
            {
               uint32_t uint = character;

               if (uint <= 0x007F)
               {
                  char c1 = (char)uint;
                  out << c1;
               }
               else if (uint <= 0x07FF)
               {
                  //110xxxxx 10xxxxxx
                  char c1 = ( 0b00011111 &
                            (char) (uint >> 6) ) |
                            0b11000000;
                            
                  char c2 = (0b00111111 &
                           (char) uint) |
                            0b10000000;
                            
                  out << c1 << c2;
               }
               else if (uint <= 0xFFFF)
               {
                  //1110xxxx 10xxxxxx 10xxxxxx
                 char c1 = (0b00001111 &
                         (char) (uint >> 12)) |
                           0b11100000;
                           
                 char c2 = (0b00111111 &
                         (char) (uint >> 6)) |
                           0b10000000;
                           
                 char c3 = (0b00111111 &
                          (char) uint) |
                           0b10000000;
                           
                 out << c1 << c2 << c3;
               }
               else if (uint <= 0x10FFFF)
               {
                  //11110xxx 10xxxxxx
                  //10xxxxxx 10xxxxxx
                  char c1 = (0b00000111 &
                          (char) (uint >> 18)) |
                            0b11110000;
                            
                  char c2 = (0b00111111 &
                          (char) (uint >> 12)) |
                            0b10000000;
                            
                  char c3 = (0b00111111 &
                          (char) (uint >> 6)) |
                            0b10000000;
                            
                  char c4 = (0b00111111 &
                           (char) uint) |
                            0b10000000;
                            
                  out << c1 << c2 << c3 << c4;
               }
               else
               {
                  out << "{"
                      << (uint64_t)character
                      << "}";
               }
            }
         }
      }

   
   };



}

#endif


