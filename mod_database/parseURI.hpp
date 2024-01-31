#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"
#include "Parser/Parser.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishWebServer {

   static Database
      database("/home/bee/bee.fish.data");
   
   Path parseURI(const char* uri) {
       
      Path path = database;
      string _uri = uri;
      
      // Add trailing /
      if (_uri[_uri.length() -1] != '/')
         _uri += '/';
         
      string segment;
 
      const auto segmentParser =
         Invoke(
            Capture(
               Character("/") and
               Repeat(
                  not Character("/"),
                  0
               ),
               segment
            ),
            [&segment, &path](Parser*) {
                   
               bool success = true;

               if ( segment != "/" )
               {
                  string _segment =
                     segment.substr(1);
           
                  path = path[_segment];
               }

               segment = "";

               return success;
            }
         );
            
      auto uriParser =
         Repeat(segmentParser, 1);
         
      uriParser.read(_uri);
      
      return path;
      
   }

}

#endif