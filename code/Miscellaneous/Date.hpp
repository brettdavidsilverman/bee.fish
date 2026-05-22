#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "../b-string/b-string-base.h"

namespace BeeFishDate
{
    
   using namespace BeeFishBString;
   
   void writeDateTime(ostream& out)
   {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
   
      out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
   
   }

   BString getDateTime() {
      std::stringstream stream;
      writeDateTime(stream);
      return stream.str();
   }

}


#endif