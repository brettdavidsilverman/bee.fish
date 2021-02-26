#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      const Match* _template;
      Match* _match;
      
   protected:
      size_t _matchedCount = 0;
        
   public:
      
      Repeat(Match* template_) :
         _template(template_),
         _match(NULL)
      {
      }
      
      virtual ~Repeat()
      {
         if (_match)
            delete _match;
            
         delete _template;
      }
      
      virtual bool match(const Char& character)
      {
      
         if (!_match)
            _match = createItem();
         
         bool matched =
            _match->match(character);
         
         if (matched)
            Match::match(character);
         
         if (_match->result() == true)
         {
         
            matchedItem(_match);
         
            _match = createItem();
         
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
      
      Repeat(const Repeat& source) :
         _template(source._template->copy())
      {
         _match = NULL;
      }
      
      virtual Match* copy() const
      {
         return new Repeat(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "Repeat", tabIndex);
         out << endl;
         out << tabs(tabIndex) << "(" << endl;
         _template->write(out, tabIndex + 1);
         out << endl;
         out << tabs(tabIndex) << ")";
      }
   };
   
   class RepeatPtr : public MatchPtr
   {
   public:
      RepeatPtr(MatchPtr _template) :
         MatchPtr(new Repeat(_template->copy()))
      {
      }
   };


};

#endif
