#ifndef BEE_FISH_PARSER__TEST_H
#define BEE_FISH_PARSER__TEST_H

#include "parser.h"

using namespace std;

namespace bee::fish::parser
{
   inline bool test();
   
   inline bool testCharacter();
   inline bool testRepeat();
   inline bool testRange();
   inline bool testWord();
   inline bool testCaseInsensitiveWord();
   inline bool testBString();
  
   
   inline bool testMatch(string label, Match& match, string text, bool result = true, string expected = "");

   inline bool test()
   {
      bool ok = true;
      
      ok &= testCharacter();
      ok &= testRepeat();
      ok &= testRange();
      ok &= testWord();
      ok &= testCaseInsensitiveWord();
      ok &= testBString();
      
      
      if (ok)
         cout << "SUCCESS";
      else
         cout << "FAIL";
         
      cout << endl;
      
      return ok;
   }
   
   inline bool testCharacter()
   {
      bool ok = true;
      
      class CharA : public Character
      {
      public:
         CharA() : Character('A')
         {
         }
      };
      
      // Character
      CharA characterMatch;
      ok &= testMatch("Character match", characterMatch, "A", true);

      CharA characterNoMatch;
      ok &= testMatch("Character no match", characterNoMatch, "B", false);
      
      Character any;
      ok &= testMatch("Character any", any, "a", true);
      
      return ok;
   }
   
   inline bool testRepeat()
   {
      bool ok = true;
      Character anyChar;
      Repeat repeat(anyChar);
      ok &= testMatch("Repeat range match", repeat, "helloworld", true, "helloworld");
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      // Character
      Range rangeMatch('a', 'z');
      ok &= testMatch("Range match", rangeMatch, "b", true, "b");

      Range rangeNoMatch('a', 'z');
      ok &= testMatch("Range no match", rangeNoMatch, "B", false);

      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      Word wordMatch(BString("Word"));
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");

      Word wordNoMatch(BString("Word"));
      ok &= testMatch("Word no match", wordNoMatch, "Wor*", false);

      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      CIWord ciWordMatch(BString("ABC"));
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");

      CIWord ciWordNoMatch(BString("ABC"));
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ", false);

      return ok;
   }
   
   inline bool testBString()
   {
      bool ok = true;
      Word runes(BString("ᛒᚢᛞᛖ"));
     
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      
      return ok;
   }
   
   inline bool testMatch(string label, Match& match, string text, bool result, const string expected)
   {
      cout << label << ":\t";
      
      Match& parser = match;
      parser._capture = true;
      
      BString exp(expected);
      
      bool ok = true;
      
      parser.read(text);
      
      if (result == true && parser.result() != true)
         ok = false;
      else if (result == false && parser.result() != false)
         ok = false;
      else if (parser.result() == true && expected.length())
      {
         
         if (exp != parser._value)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL" << endl;
         cout << "\tExpect\t" << exp << endl;
         cout << "\tGot\t" << parser._value << endl;
         cout << parser << endl;
      }
      
      return ok;
   }

}

#endif
