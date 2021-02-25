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
   inline bool testOr();
   inline bool testNot();
   inline bool testRules();
   inline bool testOptional();
   
   inline bool testMatch(string label, Match& match, string text, bool result = false, string expected = "");

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
      ok &= testOr();
      ok &= testNot();
      ok &= testRules();
      
      ok &= testOptional();
      
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
      ok &= testMatch("Character match", characterMatch, "A", true, "A");

      CharA characterNoMatch;
      ok &= testMatch("Character no match", characterNoMatch, "B");
      
      Character any;
      ok &= testMatch("Character any", any, "a", true, "a");
      
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
      ok &= testMatch("Range no match", rangeNoMatch, "B");

      return ok;
   }
   
   inline bool testWord()
   {
      bool ok = true;
      
      // Character
      Word wordMatch("Word");
      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");

      Word wordNoMatch("Word");
      ok &= testMatch("Word no match", wordNoMatch, "Wor*");

      return ok;
   }
   
   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      // Character
      CIWord ciWordMatch("ABC");
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");

      CIWord ciWordNoMatch("ABC");
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ");

      return ok;
   }
   
   inline bool testBString()
   {
      bool ok = true;
      Word runes("ᛒᚢᛞᛖ");
     
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      
      return ok;
   }
   
   inline bool testAnd()
   {
      bool ok = true;
      
      And testAnd(
         MatchPtr(new Character('a')),
         MatchPtr(
            new And(
               MatchPtr(new Character('b')),
               MatchPtr(new Character('c'))
            )
         )
      );
      
      ok &= testMatch("Simple 'and' match", testAnd, "abc", true, "abc");

      And testAndNoMatch(testAnd);
      
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "abz");
  
      return ok;
   }
   
   inline bool testOr()
   {
      bool ok = true;
      
      Or testOr(
         MatchPtr(new Word("true")),
         MatchPtr(new Word("false"))
      );
      
      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      
      Or testOrNoMatch(testOr);
      
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      return ok;
   }
   
   inline bool testNot()
   {
      bool ok = true;
      
      Not testNot(MatchPtr(new Word("true")));
      
      ok &= testMatch("Simple 'not' match", testNot, "false", true);
      
      Not testNotNoMatch(testNot);
      
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "true", false);
      
      return ok;
   }
   
   inline bool testRules()
   {
      bool ok = true;
      
      MatchPtr word1 = WORD("Hello");
      word1->_capture = true;
      MatchPtr testAndRule =
         word1 and 
         WORD("World") and
         C('.');
      
      ok &= testMatch("Rule for 'and' match", *testAndRule, "HelloWorld.", true, "HelloWorld.");
      
      ok &= word1->_value == "Hello";
      
      MatchPtr testOrRule =
         WORD("true") or
         WORD("false") or
         WORD("null");
         
      
      Or testOrRule1((Or&)*testOrRule);
      Or testOrRule2((Or&)*testOrRule);
      Or testOrRule3((Or&)*testOrRule);
      Or testOrRuleNoMatch((Or&)*testOrRule);
      
      ok &= testMatch("Rule for 'or' match first", testOrRule1, "true", true, "true");
      ok &= testMatch("Rule for 'or' match second", testOrRule2, "false", true, "false");
      ok &= testMatch("Rule for 'or' match third", testOrRule3, "null", true, "null");
      ok &= testMatch("Rule for 'or' no match", testOrRuleNoMatch, "bee");
      
      return ok;
   }
   
   inline bool testOptional()
   {
   
      bool ok = true;
      
      MatchPtr testOptional =
         WORD("one") and
         OPTIONAL(
            WORD("two"),
            WORD("three")
         );
      
      MatchPtr testOptional123 = testOptional->copy();
      
      ok &= testMatch("Optional one two three match", *testOptional123, "onetwothree", true, "onetwothree");
      
      MatchPtr testOptional13 = testOptional->copy();
      
      ok &= testMatch("Optional one three match", *testOptional13, "onethree", true, "onethree");
      
      return ok;
      
   }
   
   inline bool testMatch(string label, Match& match, string text, bool result, const string expected)
   {
      cout << label << ":\t";
      
      Match& parser = match;
      parser._capture = true;
      
      bool ok = true;
      
      parser.read(text);
      
      if (result == true && parser._result != true)
         ok = false;
      else if (result == false && parser._result != false)
         ok = false;
      else if (parser._result == true && expected.length())
      {
         
         if (parser._value != expected)
            ok = false;
      }
      
      if (ok)
         cout << "ok" << endl;
      else
      {
         cout << "FAIL " << parser._result << endl;
         cout << "\tTested\t" << text << endl;
         cout << "\tExpect\t" << expected << endl;
         cout << "\tGot\t" << parser._value << endl;
         cout << "\t" << parser << endl;
      }
      
#ifdef DEBUG
      cerr << parser << endl;
#endif
      return ok;
   }

}

#endif
