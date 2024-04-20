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
  
   cout << "bee.fish.id"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << ID_VERSION
           << endl;
           
           
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
        
        
/*
      Data data;
      if (fromKey) {
         data = Data::fromBase64(line);
         BitStream stream(data);
         BString name;
         stream >> name;
         cout << "BString from base 64:"
              << name
              << endl;
      }
      else {
         BitStream stream;
         stream << line;
         
         cout << stream.bstr() << endl;
         data = stream.toData();
         cout << data.bstr() << endl;
         BString base64 = data.toBase64();
         cout << "Base64 from BString"
              << endl
              << base64
              << endl;
      }
*/

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

