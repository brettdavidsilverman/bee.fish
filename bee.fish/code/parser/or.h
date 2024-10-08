#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"

namespace BeeFishParser {

   class Or : public Match {
   public:
      vector<Match*> _inputs;
      Match* _item = nullptr;
      
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
         
         for ( auto
                 it  = _inputs.begin();
                 it != _inputs.end();
                ++it
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
         {
            success();
         }
         else if ( result() == nullopt && 
                   !matched )
         {
            fail();
         }
         
         return matched;
         
      }
      
      virtual void eof(Parser* parser)
      {
         setup(parser);
         
         if (result() != nullopt)
            return;
            
         _item = nullptr;
         
         for ( auto item : _inputs)
         {
         
            if (item->result() == nullopt)
               item->eof(parser);
               
            if (item->result() == true)
            {
               _item = item;
               success();
               return;
            }
         }

         fail();
            
         Match::eof(parser);
            
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
      
      virtual bool matched() const {
         return
            (_item &&
            _item->result() == true);
      }
      
   };


};

#endif
