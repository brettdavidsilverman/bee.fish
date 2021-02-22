#include <iostream>
#include <memory>
#include "version.h"
#include "parser.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;

MatchPtr operator and (MatchPtr first, MatchPtr second)
{
   return MatchPtr(new And(first, second));
}

#define WORD(word) MatchPtr(new Word(word))

int main(int argc, char* argv[]) {
   
   clog << "bee.fish.server.parser "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;
   MatchPtr word1 = WORD("Hello");
   MatchPtr testAnd =
      word1 and 
      WORD("World");
   word1->_capture = true;
   
   testAnd->read("HelloWorld");
   cout << word1->_value << endl;
   return 0;
   if (!test())
      return 1;
   return 0;
   //bool success = true;
   //bool success = request.read("\"蓮书厙蹦㦕乥厙哦哦哦哦厙乥㦓餹鎙㥔锹厕㤹㣕㕍協퍍㓌쪐\\u0000\"");
  // success = request.read("\"\n\"");
 
   Word parser(BString("ᛒᚢᛞᛖ"));
   //cout << endl << "Reading from stdin." << endl;
   //parser.read(cin);
 
   if (parser.result() == true)
   {
      cout << endl
           << "ok joe"
           << endl;
   }
   else
   {
      cout << endl << "Fail" << endl;
   }
   
 
   return 0;
   
}