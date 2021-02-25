#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "../id/id.h"
#include "utf-8.h"
#include "b-string.h"

using namespace std;

using namespace bee::fish::server;

namespace bee::fish::parser {

   inline ostream& operator <<
   (ostream& out, const optional<bool>& ok)
   {
      if (ok == true)
         out << "true";
      else if (ok == false)
         out << "false";
      else
         out << "?";
         
      return out;
   }
   
   class Match;
   typedef shared_ptr<Match> MatchPtr;
   
   class Match
   {
   public:

      optional<bool> _result = nullopt;
     
      inline static const Char EndOfFile = -1;
      
      bool _capture = false;
      
      BString _value;
      
      Match()
      {
      }
      
      virtual bool match(const Char& character)
      {
         if (_capture)
            _value.push_back(character);
            
         return true;
      }
      
      virtual void write(ostream& out) const = 0;
   
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }
      
      void read(
         istream& input,
         bool last = true
      )
      {
      
         Char character;
         bool matched = false;
         
         while (!input.eof())
         {
            if (getNext(input, character))
            {
               matched = match(character);
#ifdef DEBUG
               cerr << "{"; UTF8Character::write(cerr, character); cerr << ":" << matched << "}";
#endif
               
               if (_result != nullopt)
                  break;
            }
            
         }

         if ( _result == nullopt &&
              last &&
              input.eof()
            )
         {
            matched = match(Match::EndOfFile);
#ifdef DEBUG
            cerr << "{"; UTF8Character::write(cerr, character); cerr << ":" << matched << "}";
#endif
         }
     
      }
      
   protected:
   
      bool getNext(
         istream& input,
         Char& character
      )
      {
         char nextChar;
         UTF8Character utf8;
            
         while ( !input.eof() &&
                 input.get(nextChar) &&
                 utf8.match(nextChar) &&
                 utf8._result == nullopt )
            ;
        
         if (utf8._result == true)
         {
            character = utf8._character;
            return true;
         }
         else
            return false;
      }
      
   public:
      virtual void read(
         const string& str,
         bool last = true
      )
      {
      
         stringstream input(str);
      
         return read(input, last);
      
      }

      friend ostream& operator <<
      (ostream& out, const Match& match) 
      {

         match.write(out);
   
         return out;

      }
   
      virtual void writeResult(ostream& out) const
      {
         out << "<" << _result << ">";
      }
   

      virtual MatchPtr copy() const = 0;
      
      virtual void reset()
      {
         _result = nullopt;
         _value.clear();
      }
      
      virtual bool isMatched()
      {
         return (_result == true);
      }
   
   };


   
}

#endif


