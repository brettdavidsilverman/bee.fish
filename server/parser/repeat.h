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

      Repeat(Match* t) :
         Match(),
         _template(t),
         _match(NULL)
      {
         _template = t;
         _match = NULL;
      }
   
      Repeat(const Repeat& source) :
         Match(source),
         _template(source._template),
         _match(NULL)
      {
      }
      
      virtual ~Repeat()
      {

         delete _template;
         
         if (_match)
           delete _match;
      }
   
   
      virtual bool match(WideChar character)
      {

         if (_match == NULL)
            _match = createItem();
         
         bool matched =
            _match->match(character);
         
         if (matched)
            Match::match(character);
      
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
      
      virtual void write(ostream& out)
      {
         out << "Repeat";
         writeResult(out);
         out << "(" << *_template << ")";
      }

   };


};

#endif
