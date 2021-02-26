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
      
      Repeat(MatchPtr template_) :
         _template(template_),
         _match(NULL)
      {
      }
      
      virtual bool match(const Char& character)
      {
      
         if (_match == NULL)
            _match = createItem();
         
         bool matched =
            _match->match(character);
         
         if (matched)
            Match::match(character);
         
         if (_match->result() == true)
         {
         
            matchedItem(*_match);
         
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
      
      virtual MatchPtr createItem()
      {
         return _template->copy();
      }
      
      virtual void matchedItem(Match& match)
      {
      }
      
      
      virtual string name()
      {
         return
            "Repeat" + 
            to_string(_matchedCount);
      }
      
      Repeat(const Repeat& source) 
      {
         _match = NULL;
      }
      
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Repeat(*this));
      }
      
   };
   
   class RepeatPtr : public MatchPtr
   {
   public:
      RepeatPtr(MatchPtr _template) :
         MatchPtr(new Repeat(_template))
      {
      }
   };


};

#endif
