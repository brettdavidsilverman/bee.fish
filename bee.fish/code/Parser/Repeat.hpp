#ifndef BEE_FISH__PARSER__REPEAT_HPP
#define BEE_FISH__PARSER__REPEAT_HPP

#include <vector>
#include <memory>
#include "../Miscellaneous/Miscellaneous.hpp"

#include "Parser.hpp"

namespace BeeFishParser
{

   class Repeat : public Parser
   {
   protected:
      Parser* _template;
      Parser* _item {nullptr};
      std::string _buffer;
      bool _nextItem {false};

   public:
      size_t _minimum {1};
      size_t _maximum {0};
      size_t _matchedCount {0};

   public:
      using Parser::read;

      Repeat(
         const Parser& templ,
         size_t minimum = 1,
         size_t maximum = 0) :
            _minimum(minimum),
            _maximum(maximum)
      {
         _template = templ.copy();
      }

      Repeat(const Repeat& source) :
         _template(
           source._template->copy()
         ),
         _minimum(source._minimum),
         _maximum(source._maximum)
      {
      }

      virtual ~Repeat()
      {
         if (_item)
           delete _item;

         delete _template;
      }
      
      virtual bool read(
         char character
      ) override
      {

         _buffer.push_back(character);

         bool matched = false;
         
         while (
            !matched
         )
         {

            if (_item == nullptr)
               _item = createItem();
            
            if (!_nextItem)
               matched = _item->read(character);
            else
               matched =
                  _item->read(_buffer);

            _nextItem = false;

            if (_item->_result == true)
            {
               ++_matchedCount;
               matchedItem(_item);
               delete _item;
               _item = nullptr;
               _buffer.clear();
               _nextItem = true;
               if (!matched) {
                  _buffer = character;
                  continue;
               }
               else
                  break;
            }
            else if (!matched ||
               _item->_result == false)
            {
               if (_item->isOptional()) {
                  delete _item;
                  _item = nullptr;
                  matched = false;
                  _nextItem = true;
                  ++_matchedCount;
                  continue;
               }
               else {
                  //setResult(false);
                  matched = false;
                  break;
               }
               
            }

               
         }

         if (!matched)
         {
            if ( _matchedCount >= _minimum &&
              (_matchedCount <= _maximum ||
               _maximum == 0) )
            {
               if (_matchedCount > 0)
                  setResult(true);
            }
            else
            {
              setResult(false);
            }
         }

         return matched;

      }

      virtual void matchedItem(
         Parser* item
      )
      {
      }

      virtual Parser*
      createItem()
      {
         return
            _template->copy();
      }

      virtual Parser* copy() const {
         return new Repeat(*this);
      }

      virtual bool isOptional() const
      override
      {
         return
            _minimum <= _matchedCount;
      }


   };

};

#endif
