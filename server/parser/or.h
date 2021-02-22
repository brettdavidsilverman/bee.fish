#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match* _item = NULL;
      size_t _index = 0;
      
   public:

      template<typename ...T>
      Or(T*... inputs) :
         Match(inputs...)
      {
      }
      
      Or(const Or& source) :
         Match(source)
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
         
            Match* item = *it;
            
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
               return matched;
            }
            else if (
               !matched ||
               (item->result() == false)
            )
            {
               delete item;
               *it = NULL;
            }
            
       
         }
      
         if (result() == nullopt && 
             !matched)
         {
            fail();
         }
         
         return matched;
      }
   
   
      virtual Match& item() {
         return *_item;
      }
   
      
      virtual size_t index()
      {
         return _index;
      }
      
			   
      virtual Match* copy() const
      {
         return new Or(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Or";
         writeResult(out);
         out << "(";
         writeInputs(out);
         out << ")";
         
      }
   };


};

#endif
