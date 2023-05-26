#ifndef BEE_FISH_PARSER__OR_HPP
#define BEE_FISH_PARSER__OR_HPP
#include <vector>
#include <memory>
#include "ArrayParser.hpp"


namespace BeeFishParser {

   class Or : public ArrayParser {
   public:

      using Parser::read;

      Or(const Or& source)
         : ArrayParser(source)
      {
      }

      Or(const Or& lhs, const Parser& rhs)
         : ArrayParser(lhs, rhs)
      {

      }

      Or(const Parser& lhs, const Parser& rhs)
         : ArrayParser(lhs, rhs)
      {
      }
      
      virtual ~Or()
      {
      }

      virtual bool read(
         bool bit
      ) override
      {
         throw std::logic_error("Should not reach here");
      }

      virtual bool read(char character)
      override
      {

         bool matched = false;
         _index = 0;
         std::shared_ptr<Parser> item = nullptr;

         for ( auto
                 it  = _inputs.begin();
                 it != _inputs.end();
                ++_index, ++it
             )
         {
         
            item = *it;
            
            if ( item->_result != std::nullopt )
               continue;

            if ( item->read(character) )
            {
               matched = true;
            }
            
            if ( item->_result == true )
            {
               break;
            }

       
         }
       
         if (item && (item->_result == true))
            setResult(true);
         else if ( _result == nullopt && 
                   !matched )
            setResult(false);
         
         return matched;

         
         
      }

      virtual Parser* copy() const {
         return new Or(*this);
      }


   };

};

#endif
