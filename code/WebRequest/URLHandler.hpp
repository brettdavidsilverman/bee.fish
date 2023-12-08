#ifndef BEE_FISH__WEB__URL_HANDLER_HPP
#define BEE_FISH__WEB__URL_HANDLER_HPP
#include "../Parser/ParserBase.hpp"
#include "../Database/Database.hpp"
#include "../DBServer/JSONPath.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishDBServer;
   
   typedef std::function<bool (std::string)> OnSegment;
/*
   class URLHandler {
   public:
      typedef std::function<bool (std::string)> OnSegment;
      OnSegment _onsegment;
      string _segment;
   public:

      URLHandler() 
      {
         _onsegment = [](string segment) {
            cerr << "SEGMENT0: " << segment << endl;
            return true;
         };
      }

      URLHandler(const URLHandler& source)
      {
         _onsegment = source._onsegment;
      }

   };
*/
}

#endif
