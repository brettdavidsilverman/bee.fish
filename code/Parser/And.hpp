#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <memory>
#include "Character.hpp"
#include "Optional.hpp"

using namespace std;

namespace BeeFishParser {

   class And : public ArrayParser {
   private:
      std::string _buffer;
      bool _nextItem{true};
   public:
      using Parser::read;

      And(const And& source)
         : ArrayParser(source)
      {
      }

      And(const And& lhs, const Parser& rhs)
         : ArrayParser(lhs, rhs)
      {
      }

      And(const Parser& lhs, const Parser& rhs)
         : ArrayParser(lhs, rhs)
      {

      }

      
      virtual ~And()
      {
      }

      virtual bool read(
         bool bit
      ) override
      {
         throw std::logic_error("Should not reach here");
      }

      virtual bool read(
         char c
      ) override
      {

         using namespace std;

         bool matched = false;

         if ( _index == _inputs.size() ) {
            setResult(false);
            return false;
         }

         _buffer.push_back(c);

         
         while (
            _index < _inputs.size()
         )
         {

            std::shared_ptr<Parser> 
               item = _inputs[_index];
            
            if (_nextItem) {

               matched = item->read(_buffer);
               _nextItem = false;
            }
            else {
               matched =
                  item->read(c);

            }

            if (item->_result == true)
            {
               ++_index;
               _buffer.clear();
               _nextItem = true;
               if (!matched) {
                  _buffer = c;
                  continue;
               }
            }
            else if (!matched ||
               item->_result == false)
            {


               if ( !matched &&
                   item->isOptional()) {
                  ++_index;
                  _nextItem = true;
                  continue;
               }
               else if ( item->_result == false) {
                  setResult(false);
                  break;
               }
               
            }

            if (matched)
               break;
            
               
         }

         if (_index == _inputs.size())
            setResult(true);

         return matched;

      }

      virtual Parser* copy() const {
         return new And(*this);
      }


   };

};

#endif
