#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"

using namespace std;

namespace BeeFishParser {

   class And : public Match {
   protected:
      vector<Match*> _inputs;
      size_t _iterator;
   public:

      template<typename ...T>
      And(T*... inputs) :
         _inputs{inputs...}
      {
         _iterator = 0;
      }
      
      virtual ~And()
      {
         for (auto it : _inputs)
         {
            Match* match = it;
            if (match)
               delete match;
         }
      }

      virtual void setup(Parser* parser) {
         if (_parser)
            return;
            
         _parser = parser;
         
         for (auto item : _inputs)
            item->setup(parser);
         _iterator = 0;
         
      }     

      virtual bool matchCharacter(const Char& character) {
      
         bool matched = false;
            
         if ( _iterator == size() ) {
            _result = false;
            return false;
         }
            
         while ( !matched &&
                 _result == nullopt )
         {

            Match* item = _inputs[_iterator];

            matched =
               item->match(_parser, character);
         
            if (item->_result == true) {
            
               if ( ++_iterator == 
                    size() ) {
                  _result = true;
               }
               
            }
            else if (item->_result == false) {
            
               _result = false;
               
            }
            
         }

         return matched;
         
      }

      size_t size() {
         return _inputs.size();
      }

      void push_back(Match* match) {
         
         if (_parser)
            match->setup(_parser);

         _inputs.push_back(match);

      }
    
      virtual void eof(Parser* parser) {
          
         setup(parser);
            
         if (result() != nullopt)
            return;
            
         bool matched = true;
         
         for (size_t index = _iterator; index < _inputs.size(); ++index)
         {
            Match* match = (*this)[index];
            
            if (match->result() == nullopt)
               match->eof(parser);
            
            if (match->result() != true && 
                !match->isOptional())
            {
               matched = false;
               break;
            }
         }
         
         if (matched) {
            success();
         }
         else {
            fail();
         }
      }
      
      virtual Match* operator[] (size_t index)
      {
         return _inputs[index];
      }
      

      
   };

};

#endif
