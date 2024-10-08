#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>

#include "../Miscellaneous/Miscellaneous.hpp"
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
         
      BeeFishMisc::outputSuccess(ok);
         
      cout << endl;
      
      return ok;
  
   }
   
   inline bool testBasics()
   {
      cout << "Test Basics" << endl;
      
      class CharA : public Character {
      public:
         CharA() : Character('a') {

         }
      };

      Match* a = new CharA();
      
      Parser parser(a);
      
      bool ok =
         parser.read("a") &&
         (parser.result() == true);

      delete a;

      BeeFishMisc::outputSuccess(ok);
      assert(ok);
      return ok;
   }
   
   inline bool testCharacter()
   {
      bool ok = true;
      
      cout << "Test Characters" << endl;
      
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
      
      BeeFishMisc::outputSuccess(ok);

      return ok;
   }
   
   inline bool testRange()
   {
      cout << "Test Range" << endl;
      
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
      
      BeeFishMisc::outputSuccess(ok);
      
      return ok;
   }


   inline bool testWord()
   {
      cout << "Test Words" << endl;
      
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
      
      BeeFishMisc::outputSuccess(ok);
      
      assert(ok);
      
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
      ok &= testMatch("Simple 'and' no match", testAndNoMatch, "ABc", false);
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
      ok &= testMatch("Repeat any character match", repeat, "helloworld", true, "helloworld");
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
      ok &= testMatch("Repeat fail 2", tests[2], "*BBB", false);
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
       
      cout << "Test Not" << endl;
      
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
   
      cout << "Test Optional" << endl;
      
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
      ok &= testMatch("Optional one match", testOptional1, "one", true, "one");
      delete testOptional1;
            
      return ok;
      
   }

   inline bool testBString()
   {
      cout << "Test BString" << endl;
      
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
      cout << "Test Capture" << endl;
      
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
      
      Match* test2 = new Capture(new WordCapture());
      
      ok &= testMatch("Capture reference", test2, "capture", true, "capture");
      
      ok &= testResult("Capture reference result", test2->value() == "capture");

      delete test2;
 
      class _Capture : public Match
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
         And* _and;
         
         _Capture() :
             _name(""),
             _value("")
         {
         }
         
         virtual void setup(Parser* parser)
         {
             
            if (_parser)
               return;
               
            _parser = parser;
            
            _match = _and = new And(
               new Capture(new Name(), _name),
               new Character(' '),
               new Capture(new Value(), _value)
            );
            
            _match->setup(parser);
            
         }
         
         
      };
      
      _Capture* _capture = new _Capture();
      Capture* capture = new Capture(_capture);
      
      ok &= testMatch("Derived Capture class", capture, "name value", true, "name value");
    
      ok &= testResult("Derived Capture class result", (_capture->_name == "name") && (_capture->_value == "value"));
      
      
      delete capture;
      
      class Item : public Word
      {
      public:
         Item() : Word("item")
         {
         }
      };
      
      ok &= testMatchDelete("Capture item", new Capture(new Item()), "item", true, "item");
    
         
      class LoadItemOnDemand :
          public LoadOnDemand<Item>
      {
      public:
         LoadItemOnDemand()
         {
         }
      };
      
      ok &= testMatchDelete("Capture load item on demand", new Capture(new LoadItemOnDemand()), "item", true, "item");
    
      class CaptureLoadOnDemand :
         public Capture
      {
      public:
          CaptureLoadOnDemand() :
             Capture(new LoadItemOnDemand())
          {
          }
      };
      
      ok &= testMatchDelete("Capture load item on demand 2 ", new CaptureLoadOnDemand(), "item", true, "item");
    
      
      BeeFishMisc::outputSuccess(ok);
      
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
            return true;
         } 
      );
      
      ok &= testMatch("Invoke", invoke, "invoke", true);
      ok &= testResult("Invoke value", invokeValue == "invoke");


      delete invoke;
      
      class Test : public Match
      {

      public:
         BString _test;
      public:
         Test() : Match()
         {
         }
         
         virtual void setup(Parser* parser)
         override
         {
            if (_parser)
               return;
               
            _parser = parser;

            _match = new Invoke(
               new Capture(
                  new Word("test")
               ),
               [this](Match* match)
               {
                  virtualFunction(match);
                  return true;
               }
            );
               
            _match->setup(parser);
            
         }
         
         virtual void virtualFunction(Match* match)
         {
            _test = match->value();
         }
      };

      Test* test = new Test();
      
      ok &= testMatch("Invoke class virtual", test, "test", true);
      ok &= testResult("Invoke class virtual value", test->_test == "test");

      delete test;
      
      assert(ok);
      
      BeeFishMisc::outputSuccess(ok);
      
      return ok;
   }
   
   inline bool testLoadOnDemand()
   {
      cout << "Testing load on demand" << endl;
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
      
      BeeFishMisc::outputSuccess(ok);
      
      return ok;
   }
   
  
   inline bool testMisc()
   {
   
      cout << "Testing misc " << endl;
      
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
      optional<bool> matched;
      parser.read("Br");
      parser.read("ett");
      
      bool multipartResult =
         multipart.matched() &&
         parser.result() == true;
         
      ok &= testResult("Multipart", multipartResult);
      
      
      Capture* error = new Capture(
         new And(
            new Word("a"),
            new Word("b")
         )
      );
      
      Parser parserError(error);
      parserError.read("az");
 
      ok &= testResult(
         "Parser Error result",
            parserError.result() == false
      );
      
      ok &= testResult(
         "Match Error result",
         error->result() == false
      );
      
      ok &= testResult(
         "Parser Error value",
         parserError.getError().length() > 0
      );
      
      delete error;
      
      BeeFishMisc::outputSuccess(ok);
      
      return ok;
   
   }
   
   
   
}

#endif
