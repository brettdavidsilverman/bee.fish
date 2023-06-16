#ifndef BEE_FISH__PARSER__TEST_HPP
#define BEE_FISH__PARSER__TEST_HPP


#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Test/Test.hpp"
#include "Parser.hpp"
#include "Number.hpp"

namespace BeeFishParser {

   using namespace BeeFishMisc;
   using namespace BeeFishTest;

   bool testUnicode();
   bool testCharacter();
   bool testReadCharacter();
   bool testAnd();
   bool testOr();
   bool testNot();
   bool testWord();
   bool testRange();
   bool testRepeat();
   bool testOptional();
   bool testComplex();
   bool testRules();
   bool testCapture();
   bool testInvoke();
   bool testNumber();

   inline bool test() {
      
      std::cout << "Testing parser" << std::endl << std::endl;

      if (!testUnicode())
         return false;

      if (!testCharacter())
         return false;

      if (!testReadCharacter())
         return false;

      if (!testAnd())
         return false;

      if (!testOr())
         return false;

      if (!testNot())
         return false;

      if (!testRange())
         return false;

      if (!testWord())
         return false;

      if (!testOptional())
         return false;

      if (!testComplex())
         return false;

      if (!testRepeat())
         return false;

      if (!testRules())
         return false;

      if (!testCapture())
         return false;

      if (!testInvoke())
         return false;

      if (!testNumber())
         return false;

      return true;
   }

   
   inline bool testUnicode() {
      
      bool success = true;

      std::cout << "testUnicode: " << std::flush;

      std::wstring_convert<
         std::codecvt_utf8<wchar_t>
      > utf8_conv;

      wchar_t unicodeCharacter = L'😃';
      std::string bytes =
         utf8_conv.to_bytes(unicodeCharacter);


      std::cout << bytes << std::endl;

      return true;
   }

   inline bool testCharacter() {

      bool success = true;

      std::cout << "testCharacter: " << std::flush;

      /*
      wchar_t character =
         UTF8Character::toWChar("😃");

      std::string copy =
         UTF8Character::fromWChar(character);

      success &= (copy == "😃");
    */
      Character earth("🌎");

      std::stringstream stream;
      stream << earth;

      success &= stream.str() == std::string("🌎");

      BeeFishMisc::outputSuccess(success);

      return success;

      
   }

   inline bool testReadCharacter() {

      bool success = true;

      std::cout << "testReadCharacter:" << std::endl;

      UTF8Character character;

      std::string string("🌎");

      std::cout << "\tread:";
      success =
         character.read(string);
      BeeFishMisc::outputSuccess(success);

      std::cout << "\tresult: ";
      success =
         (character._result == true);
      BeeFishMisc::outputSuccess(success);

      std::cout << "\tcharacter: ";
      success = 
         (character._chars == "🌎");
      BeeFishMisc::outputSuccess(success);

      std::cout << "\t🌎";

      character = UTF8Character("🌎");
      success = success && 
         character.read(string) &&
         character._result == true &&
         character._chars == "🌎";

      character = UTF8Character("a");

      success = success && 
         character.read("a") &&
         character._result == true &&
         character._chars == "a";

      BeeFishMisc::outputSuccess(success);


      return success;
      
   }

   inline bool testAnd() {

      bool success = true;

      std::cout << "testAnd: " << std::endl;

      auto parser = []() {
         return
            Character("a") and
            Character("b");
      };
      
      success &= testPattern(
         parser(),
         "ab",
         true
      );

      success &= testPattern(
         parser(),
         "a",
         nullopt
      );

      success &= testPattern(
         parser(),
         "b",
         false
      );

 
      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testOr() {

      bool success = true;

      std::cout << "testOr: " << std::flush;

      Character character1("a");
      Character character2("b");

      Or _or(character1, character2);

      std::string stream("b");

      success = success &&
         _or.read(stream) &&
         _or.result() == true;

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testNot() {

      bool success = true;

      std::cout << "test Not:\"hello\"" << std::endl;

      const auto word =
         not Word("hello");

      success &= testPattern(
         word,
         "world",
         true
      );
 
      success &= testPattern(
         word,
         "hello",
         false
      );

      const auto newLine =
         Word("\r\n") or Word("\n");
/*
      const auto newLine =
         Character("\r") or
         Character("\n");
*/
      string value;
      auto line = Capture(
         Repeat(not newLine),
         value
      ) and newLine;

      success &= testPattern(
         line,
         "value\r\n",
         true
      );
/*
      success &= testValue(
         "value",
         value
      );
*/
      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testWord() {

      bool success = true;

      std::cout << "testWord: " << std::endl;

      std::string string("Hello 🌎");

      auto parser = [string]() {
         return Word(string);
      };

      auto word = parser();
      std::stringstream stream;
      stream << word;

      success = success &&
         stream.str() == string;
       
      success &= testPattern(
         parser(),
         string,
         true
      );
      
      success &= testPattern(
         parser(),
         "Hello ☀️",
         false
      );
       
      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testRange() {

      bool success = true;

      std::cout << "testRange: " << std::endl;

      auto _Parser = []() {
         return Range("a", "z");
      };

      
      success &= testPattern(
         _Parser(),
         "a",
         true
      );

      success &= testPattern(
         _Parser(),
         "z",
         true
      );

      success &= testPattern(
         _Parser(),
         "1",
         false
      );


      BeeFishMisc::outputSuccess(success);

      return success;
   }

   inline bool testOptional() {

      bool success = true;

      std::cout << "testOptional:" << std::endl;

      auto parser = []() {
     
         Character a("a");
         Character b("b");
         Character c("c");
 
         return Optional(b) and c;
      };
    
      success &= testPattern(
         parser(),
         "bc",
         true
      );

      success &= testPattern(
         parser(),
         "c",
         true
      );

      success &= testPattern(
         parser(),
         "ac",
         false
      );
      
      
      auto parser2= []() {
         return
            Optional(Word("Start")) and
            Optional(Word("Stop")) and
            Word("Stammer");
      };

      success &= testPattern(
         parser2(),
         "StartStopStammer",
         true
      );

      success &= testPattern(
         parser2(),
         "StartStammer",
         true
      );

      success &= testPattern(
         parser2(),
         "StopStammer",
         true
      );
  
      success &= testPattern(
         parser2(),
         "Stammer",
         true
      );

      BeeFishMisc::outputSuccess(success);


      return success;

   }

   inline bool testComplex() {

      bool success = true;

      std::cout << "testComplex: " << std::flush;
      
      Character a("a");
      Character b("b");
      Not _notb(b);

      And _and(
         a,
         _notb
      );

      Parser& parser = _and;

      success = success &&
         testPattern(
            parser,
            "ac",
            true
         );

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testRepeat() {

      bool success = true;

      std::cout << "testRepeat:" << std::endl;


      const Range aToZ("a", "z");


      success &=
         testPattern(
            Repeat(aToZ, 1),
            "brett",
            std::nullopt
         );

      success &=
         testPattern(
            Repeat(aToZ, 1),
            "1",
            false
         );

      success &=
         testPattern(
            Repeat(aToZ, 0),
            "2",
            nullopt
         );

      auto StringParser = []() {
         const Character quote("'");
         return
            quote and
            Repeat(not quote) and
            quote;
      };
         
      success &= testPattern(
         StringParser(),
         "'hello world'",
         true
      );
         
      success &= testPattern(
         StringParser(),
         "'hello world",
         std::nullopt
      );

      success &= testPattern(
         StringParser(),
         "hello world",
         false
      );

      success &= testPattern(
         Repeat(
            Character(",") and
            Repeat(Range("0", "9"))
         ),
         ",1,2,3",
         std::nullopt
      );

      auto SpaceParser = []() {
         const Character space(" ");
         const Character tab("\t");
         const Or blank = space or tab;
         return
            Repeat(not space) and
            space;
      };
         
      success &= testPattern(
         SpaceParser(),
         "Brett ",
         true
      );

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testRules() {

      bool success = true;

      std::cout << "testRules: " << std::flush;

      Character a("a");
      Character b("b");
      Character c("c");
      Character z("z");

      
      Not _notb(b);

      auto parser =
         (a and not b) or (c and z);

      success = success &&
         parser.read("cz") &&
         (parser.result() == true);

      auto parser2 =
         a and b and c and z;

      success = success &&
         parser2.read("abcz") &&
         (parser2.result() == true);

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testCapture() {

      bool success = true;

      std::cout << "testCapture: " << std::flush;

      std::string myName;

      auto parser = Capture(
         Word("Brett") and
         Character(" ") and
         Optional(
            Word("David") and
            Character(" ")
         ) and
         Word("Silverman"),
         myName
      );

      success = parser.read("Brett David Silverman");
      //success &=
      //   (myName == "Brett David Silverman");

      BeeFishMisc::outputSuccess(success);


      return success;
   }


   inline bool testInvoke() {

      bool success = true;

      std::cout << "testInvoke: " << std::flush;

      std::string myName;

      auto parser = Capture(
         Word("Brett") and
         Character(" ") and
         Optional(
            Word("David") and
            Character(" ")
         ) and
         Word("Silverman"),
         myName
      );

      string value;

      Invoke invoke(
         parser,
         [&value](Parser* parser) {
            Capture* capture =
               dynamic_cast<Capture*>(parser);
            value = capture->value();
            return true;
         }
      );

      success = testPattern(
         invoke,
         "Brett David Silverman",
         true
      );
      
      success = testValue(
         "Brett David Silverman",
         value
      );

      BeeFishMisc::outputSuccess(success);

      return success;
   }

   inline bool testNumber() {

      bool success = true;

      std::cout << "testNumber: " << std::flush;

      std::string myName;

      success &= testPattern(
         NumberExample(),
         "1e+1",
         nullopt
      );

      BeeFishMisc::outputSuccess(success);


      return success;
   }
}

#endif