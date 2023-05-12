#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>

#include "misc.h"

#include "parser.h"

#include "../test/test.h"

using namespace std;
using namespace BeeFishTest;

namespace BeeFishParser {
   

   
   bool testBasics();
   bool testCharacter();

   bool testRange();
   bool testWord();
   bool testCaseInsensitiveWord();
   bool testAnd();
   bool testOr();
   bool testRepeat();

   bool testNot();
   bool testOptional();
   bool testBString(); 
   bool testCapture();
   bool testInvoke();
   bool testLoadOnDemand();
   bool testMisc();
   
   inline bool test() 
   {
    
      bool ok = true;
      
      ok &= testBasics();

      ok &= testCharacter();
      
      ok &= testRange();
      ok &= testWord();
      ok &= testCaseInsensitiveWord();
      ok &= testAnd();
      ok &= testOr();
      ok &= testRepeat();
      ok &= testNot();
      ok &= testOptional();
      ok &= testBString();
      ok &= testCapture();
      ok &= testInvoke();
      ok &= testLoadOnDemand();
      ok &= testMisc();

      if (ok)
         cout << "SUCCESS";
      else
         cout << "FAIL";
         
      cout << endl;
      
      return ok;
  
   }
   
   inline bool testBasics()
   {
      cout << "Test basics:\t";
      
      class CharA : public Character {
      public:
         CharA() : Character('a') {

         }
      };

      Match* _a = new CharA();
      
      Parser parser(*_a);
      
      bool ok =
         parser.read("a") &&
         (parser.result() == true);

      delete _a;

      if (ok)
         cout << "ok" << endl;
      else
         cout << "FAIL" << endl;
      
      
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
      Match* characterMatch = new Capture(new CharA());
      ok &= testMatch("Character match", characterMatch, "A", true, "A");
      delete characterMatch;
      
      Match* characterNoMatch = new Capture(new CharA());
      ok &= testMatch("Character no match", characterNoMatch, "B");
      delete characterNoMatch;
      
      Match* any = new Capture(new Character());
      ok &= testMatch("Character any", any, "a", true, "a");
      delete any;
      
      return ok;
   }
   
   inline bool testRange()
   {
      bool ok = true;
      
      class atoz : public Range {
      public:
         atoz() : Range('a', 'z') {

         }
      };

      // Character
      Match* rangeMatch = new Capture(new atoz());

      ok &= testMatch("Range match", rangeMatch, "b", true, "b");
      delete rangeMatch;
      
      Match* rangeNoMatch = new Capture(new atoz());

      ok &= testMatch("Range no match", rangeNoMatch, "B");
      delete rangeNoMatch;
      
      return ok;
   }


   inline bool testWord()
   {
      bool ok = true;

      class WordWord : public Word {
      public:
         WordWord() : Word("Word") {

         }
      };

      // Character
      Match* wordMatch = new Capture(new WordWord());

      ok &= testMatch("Word match", wordMatch, "Word", true, "Word");
      delete wordMatch;
      
      Match* wordNoMatch = new Capture(new WordWord());
      ok &= testMatch("Word no match", wordNoMatch, "Wor*");
      delete wordNoMatch;
      
      return ok;
   }

   inline bool testCaseInsensitiveWord()
   {
      bool ok = true;
      
      class ABC : public CIWord {
      public:
         ABC() : CIWord("ABC") {

         }
      };

      // Character
      Match* ciWordMatch = new Capture(new ABC());
      ok &= testMatch("Case insensitive Word match", ciWordMatch, "abc", true, "abc");
      delete ciWordMatch;
      
      Match* ciWordNoMatch = new Capture(new ABC());
      ok &= testMatch("Case insensitive Word no match", ciWordNoMatch, "abZ");
      delete ciWordNoMatch;
      
      return ok;
   }

   inline bool testAnd()
   {
      bool ok = true;
      
      class CharA : public Character {
      public:
         CharA() : Character('A') {

         }    
      };

      class CharB : public Character {
      public:
         CharB() : Character('B') {

         }    
      };

      class CharC : public Character {
      public:
         CharC() : Character('C') {

         }    
      };

      class ABC1 : public And {
      public:
         ABC1() : And() {

         }

         virtual void setup(Parser* parser) {
            _inputs = {
               new CharA(),
               new CharB(),
               new CharC()
            };
            And::setup(parser);


         }

      };

      Match* testAnd = new Capture(new ABC1());
      ok &= testMatch("Simple 'and' match", testAnd, "ABC", true, "ABC");
      delete testAnd;
      
      Match* testAndNoMatch = new Capture(new ABC1());
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "ABc");
      delete testAndNoMatch;
      
      class ABC2 : public And {
      public:
         ABC2() : And(
            new CharA(),
            new CharB(),
            new CharC()
         )
         {

         }
      };

      Match* testAnd2 = new Capture(new ABC2());
      ok &= testMatch("Simple 'and' match 2", testAnd2, "ABC", true, "ABC");
      delete testAnd2;
      
      Match* testAndNoMatch2 = new Capture(new ABC2());
      ok &= testMatch("Simple 'and' no match 2", testAndNoMatch2, "ABc");
      delete testAndNoMatch2;

      return ok;
   }

   inline bool testOr()
   {
      bool ok = true;
      
      class TrueOrFalse : public Or {
      public:
         TrueOrFalse() : Or(
            new Word("true"),
            new Word("false")
         ) 
         {

         }
      };

      Match* testOr = new Capture(new TrueOrFalse());
      ok &= testMatch("Simple 'or' match", testOr, "true", true, "true");
      delete testOr;
      
      Match* testOrNoMatch = new Capture(new TrueOrFalse());
      ok &= testMatch("Simple 'or' no match", testOrNoMatch, "maybe");
      
      delete testOrNoMatch;
      
      class BrettOrSilverman : public Or {
      public:
         BrettOrSilverman() : Or() {

         }

         void setup(Parser* parser) {
            _inputs = {
               new Word("Brett"),
               new Word("Silverman")
            };
            Or::setup(parser);
         }
      };

      Match* or1 = new Capture(new BrettOrSilverman());
      Match* or2 = new Capture(new BrettOrSilverman());
      Match* or3 = new Capture(new BrettOrSilverman());
      
      ok &= testMatch("Or first", or1, "Brett", true, "Brett");
      ok &= testMatch("Or second", or2, "Silverman", true, "Silverman");
      ok &= testMatch("Or fail", or3, "Dale");

      delete or1;
      delete or2;
      delete or3;
      
      return ok;
   }

   inline bool testRepeat()
   {
      bool ok = true;

      cout << "Test Repeat" << endl;

      Match* repeat = new Capture(new Repeat<Character>());
      ok &= testMatch("Repeat any character match", repeat, "helloworld", BeeFishMisc::nullopt, "helloworld");
      delete repeat;
      
      
      class CharStar : public Character {
      public:
         CharStar() : Character('*') {

         }
      };

      class CharB : public Character {
      public:
         CharB() : Character('B') {

         }
      };

      class StarB : public And  {
      public:
         StarB() : And(
            new CharStar(),
            new Repeat<CharB>(3, 4),
            new CharStar()
         )
         { }
      };
      
      Match* tests[] =
      {
         new Capture(new StarB()),
         new Capture(new StarB()),
         new Capture(new StarB()),
         new Capture(new StarB()),
         new Capture(new StarB())
      };
      
      ok &= testMatch("Repeat", tests[0], "*BBB*", true, "*BBB*");
      ok &= testMatch("Repeat fail 1", tests[1],  "*BB*");
      ok &= testMatch("Repeat fail 2", tests[2], "*BBB", BeeFishMisc::nullopt);
      ok &= testMatch("Repeat fail 3", tests[3], "*BBBBB*");
      ok &= testMatch("Repeat success 4", tests[4], "*BBBB*", true);

      delete tests[0];
      delete tests[1];
      delete tests[2];
      delete tests[3];
      delete tests[4];
      
      class StarBEmpty : public And  {
      public:
         StarBEmpty() : And(
            new CharStar(),
            new Repeat<CharB>(0),
            new CharStar()
         )
         { }
      };

      Match* repeatEmpty = new Capture(new StarBEmpty());
      ok &= testMatch("Repeat empty", repeatEmpty, "**", true, "**");
      delete repeatEmpty;
      
      return ok;
   }
   
   
   inline bool testNot()
   {
      bool ok = true;
      
      class ABC : public Word {
      public:
         ABC() : Word("ABC")
         {

         }
      };

      Match* testNot = new Not(new ABC());
      ok &= testMatch("Simple 'not' match", testNot, "abc", true);
      delete testNot;
      
      Match* testNotNoMatch = new Not(new ABC());
      ok &= testMatch("Simple 'not' no match", testNotNoMatch, "ABC", false);
      delete testNotNoMatch;
      
      class Notatoz : public Not {
      public:
         Notatoz() : Not(
            new Range('a', 'z')
         ) {

         }
      };

      Match* _not1 = new Capture(new Notatoz());
      ok &= testMatch("Not range match", _not1, "A", true);
      delete _not1;
      
      Match* _not2 = new Capture(new Notatoz());
      ok &= testMatch("Not range no match", _not2, "a");
 
      delete _not2;
      
      return ok;
   }


   inline bool testOptional()
   {
   
      bool ok = true;
      
      class OneOptTwo : public And {
      public:
         OneOptTwo() : And(
            new Word("one"),
            new Optional(
               new Word("two")
            )
         ) {

         }
      };

      
      Match* testOptional12 = new Capture(new OneOptTwo());
      ok &= testMatch("Optional one two match", testOptional12, "onetwo", true, "onetwo");
      delete testOptional12;
      
      Match* testOptional1 = new Capture(new OneOptTwo());
      ok &= testMatch("Optional one match", testOptional1, "one", BeeFishMisc::nullopt, "one");
      delete testOptional1;
            
      return ok;
      
   }

   inline bool testBString()
   {
      bool ok = true;
  
      class Runes : public Word {
      public:
         Runes() : Word("ᛒᚢᛞᛖ")
         {

         }
      };

      Match* runes = new Capture(new Runes());
      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 1", runes, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      delete runes;
      
      class Runes2 : public Word {
      public:
         Runes2() : Word(BString("ᛒᚢᛞᛖ")) {

         }
      };

      Match* runes2 = new Capture(new Runes2());

      ok &= testMatch("Test runes BString ᛒᚢᛞᛖ match 2", runes2, "ᛒᚢᛞᛖ", true, "ᛒᚢᛞᛖ");
      delete runes2;
      
      return ok;
   }
   
   inline bool testCapture()
   {
      bool ok = true;

      class WordCapture : public Word {
      public:
         WordCapture() : Word("capture") {

         }
      };

      Match* test1 = new Capture(new WordCapture());
      
      ok &= testMatch("Capture simple", test1, "capture", true, "capture");
      ok &= testResult("Capture simple result", test1->value() == "capture");

      delete test1;
      
      class _Capture : public And
      {
         class Name : public Word {
         public:
            Name() : Word("name") {

            }
         };

         class Value : public Word {
         public:
            Value() : Word("value") {

            }
         };

      public:
         BString _name;
         BString _value;
         
         
         _Capture() : And(
            new Capture(new Name(), _name),
            new Character(' '),
            new Capture(new Value(), _value)
         )
         {

         }
         
         
      };
      
      _Capture* _capture;
      Capture* capture = new Capture(_capture = new _Capture());
      
      ok &= testMatch("Capture class", capture, "name value", true, "name value");
      ok &= testResult("Capture class result", (_capture->_name == "name") && (_capture->_value == "value"));
      
      delete capture;
      
      return ok;
   }
   
   inline bool testInvoke()
   {
      bool ok = true;

      cout << "Testing invoke" << endl;

      class WordInvoke : public Word {
      public:
         WordInvoke() : Word("invoke") {

         }
      };

      // Invoke
      BString invokeValue;
      Invoke* invoke = new Invoke(
         new Capture(new WordInvoke()),
         [&invokeValue](Match* item)
         {
            invokeValue = item->value();
         } 
      );
      
      ok &= testMatch("Invoke", invoke, "invoke", true);
      ok &= testResult("Invoke value", invokeValue == "invoke");

      delete invoke;
      
      class Test : public Invoke
      {
      protected:
         class WordTest : public Word {
         public:
            WordTest() : Word("test") {

            }
         };

      public:
         BString _test;
      public:
         Test() : Invoke()
         {
         }
         
         virtual void setup(Parser* parser)
         {
            _match = new Capture(new WordTest());
            _function =
               [this](Match* match)
               {
                  this->virtualFunction();
               };
            Invoke::setup(parser);
            
         }
         
         virtual void virtualFunction()
         {
            _test = _match->value();
         }
      };

      Test* testParser = new Test();

      ok &= testMatch("Invoke class virtual", testParser, "test", true);
      ok &= testResult("Invoke class virtual value", testParser->_test == "test");

      delete testParser;
      
      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      bool ok = true;

      class Brett;

      // Load on demand
      Capture* loadOnDemand = new Capture(
         new And(
            new LoadOnDemand<Brett>(),
            new Word("David")
         )
      );

      class Brett : public Word {
      public:
         Brett() : Word("Brett") {

         }
      };

      ok &= testMatch("Load on demand", loadOnDemand, "BrettDavid", true, "BrettDavid"); 
 
      delete loadOnDemand;
      
      return ok;
   }
   
  
   inline bool testMisc()
   {
   
      bool ok = true;
      
      Match* c;
      
      Capture* _and = new Capture(
         new And(
            new Word("a"),
            new Word("b"),
            (c = new Capture( new Word("c")) )
         )
      );
      
      ok &= testMatch("And",
         _and,
         "abc", true, "abc"
      );
      
      ok &= testResult("And c", c->value() == "c");
      delete _and;
      
      // Multipart
      Capture multipart(new Word("Brett"));
      Parser parser(multipart);
      BeeFishMisc::optional<bool> matched;
      parser.read("Br");
      parser.read("ett");
      
      bool multipartResult =
         multipart.matched() &&
         parser.result() == true;
         
      ok &= testResult("Multipart", multipartResult);
      
      
      return ok;
   
   }
   
   
   
}

#endif
