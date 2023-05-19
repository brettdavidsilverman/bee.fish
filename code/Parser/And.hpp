#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <memory>
#include "Parser.hpp"
#include "Character.hpp"

using namespace std;

namespace BeeFishParser {

   class And : public Character {
   protected:
      std::vector< std::shared_ptr<Parser> > _inputs;
      size_t _index {0};

   public:
      using Character::read;

      And(const And& source) {
         for (const std::shared_ptr<Parser> parser : source._inputs) {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }
      }

      And(const And& lhs, const Parser& rhs)
      {

         for ( std::shared_ptr<Parser> parser :
               lhs._inputs )
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
         const Character& character
      ) override
      {
         bool matched = false;

         if ( _index == _inputs.size() ) {
            _result = false;
            return false;
         }
            
         while ( !matched &&
                 _result == std::nullopt )
         {

            std::shared_ptr<Parser> item =
               _inputs[_index];

            matched =
               item->read(character);
         
            if (item->_result == true) {
            
               if ( ++_index == 
                    _inputs.size() )
               {
                  setResult(true);
               }
               
            }
            else if (item->_result == false) {
            
               setResult(false);
               
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
