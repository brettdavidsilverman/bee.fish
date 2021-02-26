#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      MatchPtr _item = NULL;
      size_t _index = 0;
      
   public:

      template<typename ...MatchPtr>
      Or(bool variadic, MatchPtr... inputs) :
         Match(variadic, inputs...)
      {
      }
      
      virtual ~Or()
      {
      }
  
      virtual bool match(const Char& character)
      {
   
         bool matched = false;
         _index = 0;
         auto end = _inputs.end();
         
         for ( auto
                 it  = _inputs.begin();
                 it != end;
                ++_index, ++it
             )
         {
         
            MatchPtr item = *it;
            
            if (!item)
               continue;

            if (item->match(character))
            {
               matched = true;
            }
            
            if (item->result() == true)
            {
               _item = item;
               success();
               break;
            }
            else if (
               !matched ||
               (item->result() == false)
            )
            {
               *it = NULL;
            }
            
       
         }
      
         if (result() == nullopt && 
             !matched)
         {
            fail();
         }
         else if (matched)
            Match::match(character);
        
         
         return matched;
      }
   
   
      virtual Match& item() {
         return *_item;
      }
   
      virtual string& value()
      {
         return item().value();
      }
      
      virtual size_t index()
      {
         return _index;
      }
      
      virtual string name()
      {
         return "Or";
      }
      
      Or(const Or& source) :
         Match(source)
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Or(*this));
      }
      
   };

   class OrPtr : public MatchPtr
   {
   public:
      template<typename ...T>
      OrPtr(T... inputs) :
         MatchPtr(new Or(true, inputs...))
      {
      }
   };
};

#endif
