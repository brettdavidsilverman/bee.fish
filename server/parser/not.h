#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match
   {
   protected:
      Match* _match;
   public:

      Not(Match* match) :
         Match(),
         _match(match)
      {
      }
   

      virtual ~Not()
      {
         delete _match;
      }

      virtual bool match(const Char& character)
      {
      
         bool matched =
            _match->match(character);
         
         if (_match->result() == false)
            success();
         else if (_match->result() == true)
            fail();
         else if (character == Match::EndOfFile) {
            success();
         }
         
         return !matched;
      
      }
   
      Not(const Not& source) :
         Match(source),
         _match(source._match->copy())
      {
      }
			   
      virtual Match* copy() const
      {
         return new Not(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Not";
         writeResult(out);
         out << "("
             << *_match
             << ")";
         
      }
   };


};

#endif
