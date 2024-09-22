#ifndef BEE_FISH_HTTPS__CREDENTIALS_H
#define BEE_FISH_HTTPS__CREDENTIALS_H

#include "../b-string/string.h"

namespace BeeFishHTTPS {

   class Credentials
   {
   public:
      BString _username;
      BString _hash;
      bool _result = false;
   public:
      Credentials(const BString& value)
      {
         vector<BString> parts =
            value.split(':');
            
         _result = (parts.size() >= 1);
            
         if (_result)
         {
        
            
            _username = parts[0];
             Data data(value);
            _hash = data.md5();
         }
 
      }
         
   };

}

#endif