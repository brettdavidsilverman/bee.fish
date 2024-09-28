#ifndef BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#define BEE_FISH_HTTPS__RESPONSE_HEADERS_H
#include <vector>
#include <filesystem>

using namespace std;

namespace BeeFishHTTPS {

   class Session;
   
   typedef multimap<string, BString>
      ResponseHeadersBase;
   
   class ResponseHeaders :
      public ResponseHeadersBase
   {
   public:
      ResponseHeaders() :
         ResponseHeadersBase(
            {
               { "access-control-allow-origin", "bee.fish" }
            }
         )
      {
      }

      void replace(string key, BString value)
      {
         erase(key);
         emplace(key, value);
      }
      
   };
   

}

#endif