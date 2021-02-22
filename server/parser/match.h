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

   class Match
   {
   protected:

      optional<bool> _result = nullopt;

   public:
      inline static const Char EndOfFile = -1;
   
      vector<Match*> _inputs;
      bool _capture = false;
      BString _value;
      
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
   
      virtual bool match(const Char& character) = 0;
      
      virtual void write(ostream& out) const = 0;
   
      virtual const optional<bool>& result() const
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
      
      virtual void read(
         istream& input,
         bool last = true
      )
      {
      
         Char character;
         while (!input.eof())
         {
            if (getNext(input, character))
            {

#ifdef DEBUG
               UTF8Character::write(cerr, character);
#endif
               bool matched = match(character);
            
               if (_capture && matched)
                  _value.push_back(character);
         
               if (result() != nullopt)
                  break;
            }
            
         }

         if ( result() == nullopt &&
              last &&
              input.eof()
            )
         {
#ifdef DEBUG
            UTF8Character::write(cerr, Match::EndOfFile);
#endif
            match(Match::EndOfFile);
         
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
                 utf8.result() == nullopt )
            ;
        
         if (utf8.result() == true)
         {
            character = utf8.character();
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
         out << "<" << result() << ">";
      }
   
      virtual void writeInputs(ostream& out) const
      {
         for (auto it = _inputs.cbegin(); 
                   it != _inputs.cend();
                 ++it)
         {
            const Match* input = *it;
            out << *input;
            if (it + 1 != _inputs.cend())
               out << ", ";
         }
      }

      virtual Match& operator[]
      (size_t index)
      {
         return *(_inputs[index]);
      }
   
   
   };


   
}

#endif


