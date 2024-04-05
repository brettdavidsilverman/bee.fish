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
            
            if ( item->result() != nullopt )
               continue;

            if ( item->match(_parser, character) )
            {
               matched = true;
            }
            
            if ( item->result() == true )
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
      
      virtual void eof(Parser* parser)
      {
         setup(parser);
         
         if (result() != nullopt)
            return;
            
         bool matched = false;
         _index = 0;
         
         for ( auto
                 it  = _inputs.begin();
                 it != _inputs.end();
                ++_index, ++it
             )
         {
         
            Match* item = *it;
            
            if (item->result() == nullopt)
               item->eof(parser);
               
            if (item->result() == true)
            {
               matched = true;
               _item = item;
               break;
            }
         }
         
         if (matched)
            success();
         else
            fail();
            
      }

      virtual void setup(Parser* parser) {
         if (_parser)
            return;
            
         _parser = parser;
         
         for (auto item : _inputs)
            item->setup(parser);
            
      }   
      
      virtual const BString& value() const
      {
         if (_item)
            return _item->value();
            
         return Match::value();
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
      
   };


};

#endif
