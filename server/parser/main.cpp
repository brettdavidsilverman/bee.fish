#include <iostream>
#include "version.h"
#include "parser.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;

int main(int argc, char* argv[]) {
   
   cout << "bee.fish.server.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
   UTF8Character anyChar;
      string str;
      str.push_back(0xCF);
      str.push_back(0x80);
   if (anyChar.match(0xCF80))
   {
      cout << anyChar.result() << endl;
   }
      
   if (!test())
      return 1;
   
   //bool success = true;
   //bool success = request.read("\"蓮书厙蹦㦕乥厙哦哦哦哦厙乥㦓餹鎙㥔锹厕㤹㣕㕍協퍍㓌쪐\\u0000\"");
  // success = request.read("\"\n\"");
 
   Word word(BString("ᛒᚢᛞᛖ"));
   Parser parser(word);
   cout << endl << "Reading from stdin." << endl;
   parser.read(cin);
 
   if (parser.result())
   {
      cout << endl << word << endl
           << "ok joe"
           << endl;
   }
   else
   {
   
      cout << endl << "Fail" << endl;

   }
   
 
   return 0;
   
}