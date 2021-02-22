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
   inline bool testAnd();
   /*
   inline bool testOr();
   inline bool testNot();
   inline bool testOptional();
   */
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
      ok &= testAnd();
      
      /*
      ok &= testOr();
      ok &= testNot();
      ok &= testOptional();
      */
      
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
      Repeat repeat(MatchPtr(new Character()));
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
      Word wordMatch("Word");
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");

      Word wordNoMatch("Word");
      ok &= testMatch("Word no match", wordNoMatch, "Wor*", false);

      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      CIWord ciWordMatch("ABC");
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");

      CIWord ciWordNoMatch("ABC");
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
   
   inline bool testAnd()
   {
      bool ok = true;
      
      And testAnd(
         MatchPtr(new Character('a')),
         MatchPtr(new Character('b')),
         MatchPtr(new Character('c'))
      );
      
      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");

      And testAndNoMatch(testAnd);
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz", false);

      return ok;
   }
   /*
   inline bool testOr()
   {
      bool ok = true;
      
      Or testOr(
         new Word("true"),
         new Word("false")
      );
      
      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      Or testOrNoMatch(testOr);
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe", false);
      
      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      Not testNot(new Word("true"));
      
      ok &= testMatch("Simple 'not' match", testNot, "false", true);
      
      Not testNotNoMatch(testNot);
      
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "true", false);
      
      return ok;
   }
   
   inline bool testOptional()
   {
   
      bool ok = true;
      
      And testOptional
      (
         new Word("one"),
         new Optional(new Word("two")),
         new Word("three")
      );
      
      And testOptional123(testOptional);
      ok &= testMatch("Optional one two three match", testOptional123, "onetwothree", true, "onetwothree");
      
      And testOptional13(testOptional);
      
      ok &= testMatch("Optional one three match", testOptional13, "onethree", true, "onethree");
      
      return ok;
      
   }
   */
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
      
#ifdef DEBUG
      cerr << parser << endl;
#endif
      return ok;
   }

}

#endif
