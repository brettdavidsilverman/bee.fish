#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <locale>
#include "Id.hpp"
#include "Test.hpp"


using namespace std;
using namespace BeeFishId;
using namespace BeeFishBString;

int main(int argc, const char* argv[]) {
   /*
   BString name = "🐝";
   BitStream stream1;
   
   stream1 << name;
   assert(stream1._count == 0);
   
   
   BString base64 = "/GTxjxPB8ZPDk4yfGTwfGTw5OIA=";
   base64 = stream1.toData().toBase64();
   
   cout << base64 << endl;
   
   BitStream stream2(Data::fromBase64(base64));
   
   
   BString name2;
   
   stream2 >> name2;
   
   assert(stream2._count == 0);
   
   cerr << "stream name: " << name2 << endl;
   
   return 0;
   */
   /*
   while (0) {
      auto nanoseconds =
            std::chrono::duration_cast
               <std::chrono::nanoseconds>
               (
                  std::chrono::system_clock
                     ::now()
                     .time_since_epoch()
               ).count();
   
      cout << nanoseconds << endl;
   }
   */
   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing Id..." << endl;
      if (!BeeFishId::test())
         return 1;
            
      return 0;
   }
   
   bool fromKey = false;
   if (hasArg(argc, argv, "-key") >= 0)
      fromKey = true;
      
   while (!cin.eof())
   {
      std::string line;
      
      if (fromKey)
         std::cout << "Key: ";
      else
         std::cout << "Name: ";
         
      getline(cin, line);

      if (line == "")
         break;

      Id id;
      
      if (fromKey)
         id = Id::fromKey(line);
      else
         id = Id(line);

      cout << id.toString() << endl;
      cout << id.key() << endl;
      
   }

   cout << "Finished" << endl;

   return 0;

}

