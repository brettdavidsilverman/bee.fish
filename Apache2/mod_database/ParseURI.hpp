#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishWebServer {

   extern Database database;
      
   Path parseURI(const char* clientIP, const char* uri) {
       
      Path path = database;
      path = path[clientIP];
      
      string _uri = uri;
      
      // Add trailing /
      if (_uri.length() == 0 ||
          _uri[_uri.length() -1] != '/')
         _uri += '/';
         
      string segment;
      std::stringstream test(_uri);
      while(std::getline(test, segment, '/'))
      {
         path = path[BString(segment)];
      }

/*
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
      */
      return path;
      
   }
   

}

#endif