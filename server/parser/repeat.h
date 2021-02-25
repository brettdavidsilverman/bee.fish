#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      MatchPtr _match;
   
   protected:
      size_t _matchedCount = 0;
  
   public:

      Repeat(MatchPtr match) :
         _match(match)
      {
      }
   
      Repeat(const Repeat& source) :
         _match(source._match->copy())
      {
      }
   
      virtual bool match(const Char& character)
      {

         bool matched =
            _match->match(character);

         if (_match->_result == true)
         {
            Match::match(character);
            matchedItem(_match);
            _match->reset();;
            ++_matchedCount;
         }
         else if (
            (_match->_result == false) ||
            (!matched) ||
            (character == Match::EndOfFile)
         )
         {
            if (_matchedCount > 0)
            {
               success();
            }
            else
            {
               fail();
            }
         }
      
         return matched;
      
      }
   
      virtual void matchedItem(MatchPtr match)
      {
      
      }
   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Repeat(*this));
      }
      
      virtual void reset()
      {
         Match::reset();
         _match->reset();
         _matchedCount = 0;
      }
      
      virtual void write(ostream& out) const
      {
         out << "Repeat";
         writeResult(out);
         out << "(" << *_match << ")";
      }

   };


};

#endif
