#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      MatchPtr _template;
      MatchPtr _match;
   
   protected:
      size_t _matchedCount = 0;
  
   public:

      Repeat(MatchPtr templatePtr) :
         Match(),
         _template(templatePtr),
         _match(NULL)
      {
      }
   
      Repeat(const Repeat& source) :
         Match(),
         _template(source._template->copy()),
         _match(NULL)
      {
      }
   
   
      virtual bool match(const Char& character)
      {

         if (_match == NULL)
            _match = createItem();
         
         bool matched =
            _match->match(character);

         if (_match->result() == true)
         {
            Match::match(character);
            matchedItem(_match);
            _match = NULL;
            ++_matchedCount;
         }
         else if (
            (_match->result() == false) ||
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
   
      virtual MatchPtr createItem()
      {
         return _template->copy();
      }
   
      virtual void matchedItem(MatchPtr match)
      {
      
      }
   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Repeat(*this));
      }
      
      virtual void write(ostream& out) const
      {
         out << "Repeat";
         writeResult(out);
         out << "(" << *_template << ")";
      }

   };


};

#endif
