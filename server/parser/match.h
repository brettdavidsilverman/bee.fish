#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <memory>

#include "utf-8.h"

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
   
			class Match {
			public:
			
			   string _value = "";
			   wstring _wvalue = L"";
			   optional<bool> _result = nullopt;
			   
			public:
			   inline static const Char EndOfFile = -1;
			   bool _capture = true;
			   
			   vector<MatchPtr> _inputs;
			   
			   Match()
			   {
			   }
			   
			   template<typename ...MatchPtr>
			   Match(bool variadic, MatchPtr... inputs) :
			      _inputs{inputs...}
			      
			   {
			
			   }
			   
			   virtual ~Match()
			   {
			   }
			   
			   Match(const Match& source)
			   {
			      
			      for (auto it = source._inputs.begin();
			                it != source._inputs.end();
			                ++it)
			      {
			         MatchPtr match = *it;
			         if (match)
			         {
			            MatchPtr copy = match->copy();
			            _inputs.push_back(copy);
			         }
			      }
			      
			   }
			   
			   virtual MatchPtr copy() const = 0;
			   /*
			   {
			      return new Match(*this);
			   }
			   */
			   
			   virtual bool match
			   (const Char& character)
			   {
			      
			      if ( _capture &&
			           character != Match::EndOfFile 
			         )
			      {
			         _value += (char)character;
			         _wvalue += (char)character;
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
             match(character);
			         
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
			   
			   virtual bool read(const string& str, bool last = true)
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
			   
			   virtual void write(ostream& out) 
			   {
			      
			      out << "{" 
			          << name() 
			          << ":{ok:\""
			          << _result
			          << "\"";
			 
			      for (auto it = _inputs.cbegin();
			                it != _inputs.cend();
			                ++it
			          )
			      {
			         out << ',';
			         MatchPtr input = *it;
			         if (input)
			            out << *input;
			         else
			            out << "null";
			      }
			      
			      out << '}'
			          << '}';
			   }
			   
			   virtual string name() {
			      return "Match";
			   }
			   
			   virtual Match& operator[]
			   (size_t index)
			   {
			      return *(_inputs[index]);
			   }
			   
			   virtual string& value()
			   {
			      return _value;
			   }
			   
			   virtual wstring& wvalue()
			   {
			      return _wvalue;
			   }
			   
			   virtual bool isOptional()
			   {
			      return false;
			   }
			   
			public:
			
			   
			   
			   static void write(ostream& out, int character)
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
			         out << (char)character;
			      }
			   }
			
			   
			};
			
			
			
}
			
#endif
			
			
