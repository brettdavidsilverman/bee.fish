#include <sstream>
#include "md5.h"

namespace BeeFishServer
{

   std::string md5(const std::string & str) {
  
      unsigned char result[MD5_DIGEST_LENGTH];
      
      MD5(
         (unsigned char*)str.c_str(),
         sizeof(char) * str.size(),
         result
      );

      std::stringstream sout;
      
      sout << std::hex << std::setfill('0');
      
      for(long long c: result)
      {
          sout << std::setw(2) << (long long)c;
      }
      
      return sout.str();
      
   }

}
