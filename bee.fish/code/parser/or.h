#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include "../misc/optional.h"
#include "match.h"

namespace BeeFishParser {

   class Or : public Match {
   public:
      vector<Match*> _inputs;
      Match* _item = nullptr;
      size_t _index = 0;
      
   public:

      template<typename ...T>
      Or(T*... inputs) :
         _inputs{inputs...}
      {
      }
      
      virtual ~Or()
      {
         for (auto match : _inputs) {
            delete match;
         }
      }
      
      virtual bool matchCharacter(const Char& character)
      {
   
         bool matched = false;
         _index = 0;
         
         for ( auto
                 it  = _inputs.begin();
                 it != _inputs.end();
                ++_index, ++it
             )
         {
         
            Match* item = *it;
            
            if ( item->_result != nullopt )
               continue;

            if ( item->match(_parser, character) )
            {
               matched = true;
            }
            
            if ( item->_result == true )
            {
               _item = item;
               break;
            }

       
         }
       
         if (_item)
            _result = true;
         else if ( _result == nullopt && 
                   !matched )
            _result = false;
         
         return matched;
         
      }
      
      

      virtual void setup(Parser* parser) {
         if (_parser)
            return;
            
         _parser = parser;
         
         for (auto item : _inputs)
            item->setup(parser);
            
      }   
      
      virtual BString& value()
      {
         if (_item)
            return _item->value();
            
         return Match::value();
      }
      
      virtual Match& item()
      {
         return *_item;
      }
      
      virtual const Match& item() const
      {
         return *_item;
      }
      
      virtual void capture(Parser* parser, char c)
      override
      {
         if (!_parser)
            setup(parser);
          
         for (auto item : _inputs)
             item->capture(parser, c);
      }
   };


};

#endif
