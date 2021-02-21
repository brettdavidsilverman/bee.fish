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

   typedef uint32_t Char;
   
   class Match
   {
   protected:

      optional<bool> _result = nullopt;
   
   public:
      inline static const Char EndOfFile = -1;
   
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
   
      virtual bool match(const Char& character) = 0;
      
      virtual void write(ostream& out) const = 0;
   
      virtual bool read(
         istream& in,
         bool last = true
      )
      {
      
         _result = nullopt;
     
         while (!in.eof())
         {
            Char character = in.get();

            if (character == Match::EndOfFile)
               break;
#ifdef DEBUG
            Match::write(cerr, character);
#endif
            match((Char)character);
         
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


