#ifndef BEE_FISH_PARSER__TEST_H
#define BEE_FISH_PARSER__TEST_H

#include "parser.h"

using namespace std;

namespace bee::fish::parser
{
   inline bool test();
   
   inline bool testCharacter();
   inline bool testRange();
   inline bool testWord();
   inline bool testCaseInsensitiveWord();
   inline bool testRepeat();
   inline bool testUTF8Character();
   
   inline bool testMatch(string label, Match& match, string text, bool result = true, wstring expected = L"");

   inline bool test()
   {
      bool ok = true;
      
      ok &= testCharacter();
      ok &= testRange();
      ok &= testWord();
      ok &= testCaseInsensitiveWord();
      ok &= testRepeat();
      ok &= testUTF8Character();
      
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
      ok &= testMatch("Character match", characterMatch, "A", true, L"A");

      CharA characterNoMatch;
      ok &= testMatch("Character no match", characterNoMatch, "B", false);
      
      Character any;
      ok &= testMatch("Character any", any, "a", true, L"a");
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      // Character
      Range rangeMatch('a', 'z');
      ok &= testMatch("Range match", rangeMatch, "b", true, L"b");

      Range rangeNoMatch('a', 'z');
      ok &= testMatch("Range no match", rangeNoMatch, "B", false);

      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      Word wordMatch(L"Word");
      ok &= testMatch("Word match", wordMatch, "Word", true, L"Word");

      Word wordNoMatch(L"Word");
      ok &= testMatch("Word no match", wordNoMatch, "Wor*", false);

      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      CIWord ciWordMatch(L"ABC");
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, L"abc");

      CIWord ciWordNoMatch(L"ABC");
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ", false);

      return ok;
   }
   
   inline bool testRepeat()
   {
      bool ok = true;
      Range range('a', 'z');
      Repeat repeat(range.copy());
      ok &= testMatch("Repeat range match", repeat, "helloworld", true, L"helloworld");
      
      return ok;
   }
   
   inline bool testUTF8Character()
   {
      bool ok = true;
      Repeat repeat(new UTF8Character());
      ok &= testMatch("UTF8 character repeat match", repeat, "helloworld", true, L"helloworld");
      
      return ok;
   }
   
   inline bool testMatch(string label, Match& match, string text, bool result, wstring expected)
   {
      cout << label << ":\t";
      
      match._capture = true;
      match.read(text);
      
      bool ok = true;
      
      if (result == true && match.result() != true)
         ok = false;
      else if (result == false && match.result() != false)
         ok = false;
      else if (match.result() == true)
      {
         if (expected != match.str())
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL" << endl << match << endl;
         wcout << "\tExpected \t" << expected << endl;
         wcout << "\tGot\t" << match.str() << endl;
      }
      
      return ok;
   }

}

#endif
