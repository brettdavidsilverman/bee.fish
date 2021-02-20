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
			
			class Match {
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
			   /*
			   {
			      return new Match(*this);
			   }
			   */
			   
			   virtual bool match
			   (WideChar character)
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
			   
			   virtual void writeResult(ostream& out) 
			   {
			      out << "<" << result() << ">";
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
			
			   
			   static void write(ostream& out, const wstring& text)
			   {
			      for (auto c : text)
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
			         out << (char)character;
			      }
			   }
			
			   
			};
			
			
			
}
			
#endif
			
			
