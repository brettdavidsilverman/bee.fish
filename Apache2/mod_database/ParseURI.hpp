#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishWebServer {

   Path parseURI(Database& database, const char* clientIP, const char* uri) {
      
      Path path(database);
      path = path[HOST]; //[clientIP];
      
      string _uri = uri;
      
      // Add trailing /
      if (_uri.length() == 0 ||
          _uri[_uri.length() - 1] != '/')
         _uri += '/';
         
      BString segment;
      std::stringstream segments(_uri);
      while(std::getline(segments, segment, '/'))
      {
         if (segment.size())
         {

            path = path[segment.decodeURI()];

         }
      }

      return path;
      
   }
   

}

#endif
