#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      Match* _template;
      Match* _match;
   
   protected:
      size_t _matchedCount = 0;
  
   public:

      Repeat(const Match& t) :
         Match(),
         _template(t.copy()),
         _match(NULL)
      {
      }
   
      Repeat(const Repeat& source) :
         Match(source),
         _template(source._template->copy()),
         _match(NULL)
      {
      }
      
      virtual ~Repeat()
      {

         delete _template;
         
         if (_match)
           delete _match;
      }
   
   
      virtual bool match(const Char& character)
      {

         if (_match == NULL)
            _match = createItem();
         
         bool matched =
            _match->match(character);

         if (_match->result() == true)
         {
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
   
      virtual Match* createItem()
      {
         return _template->copy();
      }
   
      virtual void matchedItem(Match* match)
      {
      
         delete match;
      
      }
   
      virtual Match* copy() const
      {
         return new Repeat(*this);
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
