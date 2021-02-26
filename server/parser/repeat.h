#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   protected:
      MatchPtr _match;
      MatchPtr _untill;
      size_t _matchedCount = 0;
      size_t _minimum = 0;
      size_t _maximum = 0;
   public:

      Repeat(
         MatchPtr match,
         MatchPtr untill,
         size_t minimum = 0,
         size_t maximum = 0
      ) :
         _match(match),
         _untill(untill),
         _minimum(minimum),
         _maximum(maximum)
      {
      }
   
      Repeat(const Repeat& source) :
         _match(source._match->copy()),
         _untill(source._untill->copy()),
         _minimum(source._minimum),
         _maximum(source._maximum)
      {
      }
   
      virtual bool match(const Char& character)
      {
               
         bool matched = false;
         
         bool matchedUntill =
            _untill->match(character);
         
         if (_untill->_result == true)
         {
            testMatchCount();
            return false;
         }
         else
         {
            if (_untill->_result == false ||
                !matchedUntill)
            {
               _untill->reset();
            }
            
            matched =
               _match->match(character);


            if (_match->_result == true)
            {
               ++_matchedCount;
               matchedItem(*_match);
               
               _match->reset();
               _untill->reset();
               
               if ( _matchedCount == _maximum )
                  success();
            }
            else if (
               (_match->_result == false) ||
               (!matched) ||
               (character == Match::EndOfFile)
            )
            {
               testMatchCount();
               return false;
            }
         }
      
         if (matched)
            Match::match(character);
            
         return matched;
      
      }
   
      virtual void matchedItem(Match& match)
      {
  
      }
   
      bool testMatchCount()
      {

         if ( _matchedCount >= _minimum &&
              ( _matchedCount <= _maximum ||
                _maximum == 0 ) )
         {
            success();
            return true;
         }
         else
         {
            fail();
            return false;
         }
      }
      
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Repeat(*this));
      }
      
      virtual void reset()
      {
         Match::reset();
         _match->reset();
         _untill->reset();
         _matchedCount = 0;
      }
      
      virtual void write(ostream& out) const
      {
         out << "Repeat";
         writeResult(out);
         out << "(" << *_match << ")";
      }

   };
   
   class RepeatPtr : public MatchPtr
   {
   public:
      RepeatPtr(
         MatchPtr match,
         MatchPtr untill,
         size_t minimum = 0,
         size_t maximum = 0
      ) : MatchPtr(
         new Repeat(
            match,
            untill,
            minimum,
            maximum
         )
      )
      {
      }
   };

};

#endif
