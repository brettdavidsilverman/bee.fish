#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include "../misc/optional.h"
#include <map>
#include <sstream>
#include <functional>

using namespace std;

namespace BeeFishParser {

   
   
   class Invoke : public Match
   {
   public:
   
      typedef std::function<void(Match*)> Function;
      Function _function;

   public:
   
      Invoke() {
      }

      Invoke(Match* match) : Match(match) {

      }

      Invoke(
         Match* match,
         Function func
      ) :
         Match(match),
         _function(func)
      {
      }

      virtual ~Invoke() {
      }

      virtual void success()
      {
         Match::success();
         if (_function)
            _function(_match);
         
      }
      
   
   };



}

#endif


