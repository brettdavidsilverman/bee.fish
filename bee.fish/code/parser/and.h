#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
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
      
      virtual void capture(Parser* parser, char c)
      override
      {
         if (!_parser)
            setup(parser);
            
         if (size() == 0)
            return;

         if (_result != false)
         {
    
            Match* item = _inputs[_iterator];
            //if (item->_result != false)
            item->capture(parser, c);
            
         }
         
         
      }
      
      virtual Match* operator[] (size_t index)
      {
         return _inputs[index];
      }
      

      
   };

};

#endif
