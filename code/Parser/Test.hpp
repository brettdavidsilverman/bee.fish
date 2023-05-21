#ifndef BEE_FISH_PARSER__TEST_HPP
#define BEE_FISH_PARSER__TEST_HPP

#include "Version.hpp"
#include "Parser.hpp"
#include "Character.hpp"
#include "And.hpp"
#include "Or.hpp"
#include "Not.hpp"
#include "Word.hpp"
#include "Range.hpp"
#include "Repeat.hpp"
#include "Optional.hpp"
#include "Rules.hpp"
#include "Capture.hpp"

namespace BeeFishParser {

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

      
      UTF8Character utf8("😃");

      std::wstring wstring = utf8.toWString();

      UTF8Character copy =
         UTF8Character::fromWString(wstring);

      success &= (copy == utf8);
      
      Character earth("🌎");

      std::stringstream stream;
      stream << earth;

      success &= stream.str() == std::string("🌎");

      BeeFishMisc::outputSuccess(success);

      return success;

      
   }

   inline bool testReadCharacter() {

      bool success = true;

      std::cout << "testReadCharacter: " << std::flush;

      UTF8Character character;

      std::string string("🌎");

      success = success && 
         character.read(string) &&
         character._result == true &&
         character._chars == string;

      character = UTF8Character("🌎");
      
      success = success && 
         character.read(string) &&
         character._result == true &&
         character._chars == string;

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

      std::cout << "testAnd: " << std::flush;

      Character a("a");
      Character b("b");

      And _and(a, b);

      success = success &&
         _and.read("ab") &&
         _and.result() == true;

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

      std::cout << "testNot: " << std::flush;

      auto Parser = []() {
         Word a("hello");
         Not _nota(a);
         return _nota;
      };

      if (success)
      {
         std::cout << "\tworld:";
         auto parser = Parser();
         parser.read("world");
         success = success &&
            parser.result() == true;
         BeeFishMisc::outputSuccess(success);
         
      }

      if (success)
      {
         std::cout << "\thello:";
         auto parser = Parser();
         parser.read("hello");
         success = success &&
            parser.result() == std::nullopt;
         BeeFishMisc::outputSuccess(success);
      }

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testWord() {

      bool success = true;

      std::cout << "testWord: " << std::flush;

      std::string string("Hello 🌎");

      Word word(string);
      std::stringstream stream;
      stream << word;

      success = success &&
         stream.str() == string;

      success = success &&
         word.read(string) &&
         word._result == true &&
         word == string;

      word = Word(string);

      success = success &&
         (word.read("Hello ☀️") == false) &&
         (word._result == false);

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testRange() {

      bool success = true;

      std::cout << "testRange: " << std::flush;

      Range aToZ("a", "z");

      Range parser = aToZ;

      success = success &&
         parser.read("a") &&
         parser._result == true;

      parser = aToZ;
      success = success &&
         parser.read("1") == false &&
         parser._result == false;

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testOptional() {

      bool success = true;

      std::cout << "testOptional:" << std::endl;

      Character a("a");
      Character b("b");
      Character c("c");

    
      if (success) {
         And parser = Optional(b) and c;
         std::string string = "bc";
         std::cout << "\t" << string << ": ";
         success =
            parser.read(string) &&
            parser.result() == true;

         BeeFishMisc::outputSuccess(success);

      }

      if (success) {
         And parser = Optional(b) and c;
         std::string string = "c";
         std::cout << "\t" << string << ": ";
         success =
            parser.read(string) &&
            parser.result() == true;

         BeeFishMisc::outputSuccess(success);

      }

      if (success) {
         And parser = Optional(b) and c;
         std::string string = "ac";
         std::cout << "\t" << string << ": " << std::flush;
         success =
            parser.read(string) == false &&
            parser.result() == false;

         BeeFishMisc::outputSuccess(success);

      }

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

      parser.read("ac");
      success = success &&
         (parser.result() == true);

      BeeFishMisc::outputSuccess(success);


      return success;
   }

   inline bool testRepeat() {

      bool success = true;

      std::cout << "testRepeat: " << std::flush;

      Range aToZ("a", "z");

      Repeat name1 = Repeat(aToZ, 1);

      success = success &&
         name1.read("brett") &&
         name1._result == std::nullopt;

      Repeat name2 = Repeat(aToZ, 1);
      success = success &&
         name2.read("1") == false &&
         name2._result == false;

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
}

#endif