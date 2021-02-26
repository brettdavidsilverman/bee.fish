#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <memory>

#include "utf-8.h"
#include "b-string.h"

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

namespace bee::fish::parser {

   class Match;
   
   typedef shared_ptr<Match> MatchPtr;
   
   template<typename T>
   shared_ptr<T> Ptr()
   {
      return shared_ptr<T>(new T());
   }
   
   class Match {
   public:
   
      BString _value;
      optional<bool> _result = nullopt;
 
   public:
      inline static const Char EndOfFile = -1;
      bool _capture = false;
      
      vector<Match*> _inputs;
      
      Match()
      {
      }
      
      template<typename ...T>
      Match(T*... inputs) :
        _inputs{inputs...}
        
      {
   
      }
      
      virtual ~Match()
      {
         for (Match* match : _inputs)
         {
            if (match)
               delete match;
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
      
      virtual bool match
      (const Char& character)
      {
        
         if ( _capture &&
              character != Match::EndOfFile 
            )
         {
            _value.push_back(character);
         }
        
         return true;
      }
   
      virtual bool read(
         istream& input,
         bool last = true
      ) 
      {
        
         _result = nullopt;
         Char character;
         while (!input.eof())
         {
            if (getNext(input, character))
            {
               if (character == Match::EndOfFile)
               {
                  break;
               }
#ifdef DEBUG   
               UTF8Character::writeEscaped(cerr, character);
#endif
               match(character);
            }
            else
               _result = false;
               
            if (_result != nullopt)
               break;
            
         }

         if ( _result == nullopt &&
            last &&
            input.eof()
          )
         {
#ifdef DEBUG
            UTF8Character::writeEscaped(cerr, Match::EndOfFile);
#endif
            match(Match::EndOfFile);
          
            return _result == true;
         }
       
         return (_result != false);
      }
      
      virtual bool read(const string& str, bool last = true)
      {
        
        istringstream in(str);
        
        return read(in, last);
        
      }
   protected:
   
      bool getNext(
         istream& input,
         Char& character
      )
      {
         int nextChar;
         UTF8Character utf8;
         
         while ( !input.eof() )
         {
            nextChar = input.get();
            
            if ((Char)nextChar == Match::EndOfFile)
            {
               character = Match::EndOfFile;
               return true;
            }
               
            utf8.match(nextChar);
            
            if (utf8._result != nullopt)
               break;
         }
      
         if (utf8._result == true)
         {
            character = utf8._character;
            return true;
         }
         else
            return false;
      }
   
   public:
      virtual optional<bool> result()
      {
         return _result;
      }
      
      virtual bool matched()
      {
         return _result == true;
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
   
         match.write(out, 0);
      
         return out;
   
      }
      
      virtual void write(ostream& out, size_t tabIndex = 0) const
      {
         writeHeader(out, "Match", tabIndex);
         out << endl;
         writeInputs(out, tabIndex);
      }
      
      virtual void writeHeader(ostream& out, string name, size_t tabIndex) const
      {
         out << tabs(tabIndex)
             << BString(name)
             << "<" << _result << ">";
      }
      
      string tabs(size_t tabIndex) const
      {
         string str;
         for(size_t i = 0; i < tabIndex; i++)
            str += "   ";
         return str;
      }
      
      virtual void writeInputs(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << "(" << endl;
         
         for (auto it = _inputs.cbegin();
                   it != _inputs.cend();
                 ++it
         )
         {
            Match* input = *it;
            if (input)
            {
               input->write(out, tabIndex + 1);
            }
            else
               out << tabs(tabIndex + 1) << "null";
           
            if ((it + 1) != _inputs.cend())
               out << ',';
               
            out << endl;
         }
        
         out << tabs(tabIndex) << ")";
      }
      
      virtual Match& operator[]
      (size_t index)
      {
         return *(_inputs[index]);
      }
   
   };
   
   
   
}
   
#endif
   
   
