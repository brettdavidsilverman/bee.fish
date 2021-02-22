#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include "version.h"
#include "match.h"
#include "character.h"
#include "utf-8.h"
#include "b-string-stream.h"
#include "b-string.h"
#include "range.h"
#include "word.h"
#include "ciword.h"
#include "repeat.h"
#include "capture.h"
/*
#include "and.h"
#include "or.h"
#include "not.h"

#include "optional.h"
#include "load-on-demand.h"
*/
using namespace std;

namespace bee::fish::parser
{

   class Parser
   {
   protected:
      Match& _match;
      UTF8Character _utf8Char;
      
   public:
      Parser(Match& match) :
         _match(match)
      {
      }
      
      virtual void read(
         BStringStream& input,
         bool last = true
      )
      {
      
         Char character;
         while (!input.eof())
         {
            input >> character;

#ifdef DEBUG
            BString::write(cerr, character);
#endif
            _match.match(character);
         
            if (_match.result() != nullopt)
               break;
         }

         if ( _match.result() == nullopt &&
              last &&
              input.eof()
            )
         {
#ifdef DEBUG
            bstring::write(cerr, Match::EndOfFile);
#endif
            _match.match(Match::EndOfFile);
         
         }
     
      }
   
      virtual void read(
         istream& input,
         bool last = true
      )
      {
         BStringStream stream(input);
         read(input, last);
      }
      
      virtual void read(
         const string& str,
         bool last = true
      )
      {
      
         BStringStream input(str);
      
         return read(input, last);
      
      }
      
      virtual const optional<bool>& result() const
      {
         return _match.result();
      }
   

   };
}

#endif