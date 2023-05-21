#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <memory>
#include "Character.hpp"
#include "Optional.hpp"

using namespace std;

namespace BeeFishParser {

   class And : public Parser {
   private:
      std::vector< std::shared_ptr<Parser> > _inputs;
      size_t _index {0};
      std::string _buffer;
   public:
      using Parser::read;

      And(const And& source) {
         for (const std::shared_ptr<Parser>& parser : source._inputs) {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }
      }

      And(const And& lhs, const Parser& rhs)
      {

         for ( auto parser : lhs._inputs )
         {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }

         _inputs.push_back(
            std::shared_ptr<Parser>(
               rhs.copy()
            )
         );

      }

      And(const Parser& lhs, const Parser& rhs)
      {
         _inputs.push_back(
            std::shared_ptr<Parser>(
               lhs.copy()
            )
         );

         _inputs.push_back(
            std::shared_ptr<Parser>(
               rhs.copy()
            )
         );

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
         char character
      ) override
      {

         using namespace std;

         bool matched = false;

         if ( _index == _inputs.size() ) {
            setResult(false);
            return false;
         }

         _buffer.push_back(character);
         std::shared_ptr<Parser> 
            item;

         bool nextItem = false;

         while (
            !matched &&
            _result == std::nullopt
         )
         {
            item = _inputs[_index];
            if (nextItem) {
               matched =
                  item->read(_buffer);
               nextItem = false;
            }
            else {
               matched =
                  item->read(character);
            }

            
            if (!matched) {
            //if (item->_result == false) {

 
               if (item->isOptional()) {
                  ++_index;
                  if ( _index == 
                       _inputs.size() )
                  {
                     setResult(true);
                  }
                  nextItem = true;
               }
               else {
                  setResult(false);
               }
               
            }
            else if (item->_result == true) {
               ++_index;
               if ( _index == 
                    _inputs.size() )
               {
                  setResult(true);
               }
               _buffer.clear();
               
            }
         }

         return matched;

      }
/*
      virtual bool read(
         const Character& character
      ) override
      {

         bool matched = false;

         if ( _lhs->_result == std::nullopt ) {
            matched = _lhs->read(character);
         }
         
         if (!matched &&
            _lhs->_result == true &&
            _rhs->_result == std::nullopt)
         {
            matched = _rhs->read(character);
         }

         if (matched) {
            if (_lhs->_result == true &&
                _rhs->_result == true )
            {
               setResult(true);
            }
         }
         
         if (_lhs->_result == false ||
             _rhs->_result == false )
         {
            setResult(false);
         }

         return matched;

         
         
      }
*/
      virtual Parser* copy() const {
         return new And(*this);
      }
      
   };

};

#endif
