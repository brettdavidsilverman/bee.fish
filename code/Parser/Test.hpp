#include "Parser.hpp"
#include "Character.hpp"
#include "And.hpp"
#include "Or.hpp"
#include "Not.hpp"
#include "Word.hpp"
#include "Rules.hpp"

namespace BeeFishParser {

   bool testUnicode();
   bool testCharacter();
   bool testReadCharacter();
   bool testAnd();
   bool testOr();
   bool testNot();
   bool testWord();
   bool testComplex();
   bool testRules();

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

      if (!testWord())
         return false;

      if (!testComplex())
         return false;

      if (!testRules())
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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

      return success;
   }

   inline bool testNot() {

      bool success = true;

      std::cout << "testNot: " << std::flush;

      Character a("a");
      Not _nota(a);
    
      Parser& parser = _nota;

      success = success &&
         parser.read("z") &&
         parser.result() == true;

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail: " << parser.result() << std::endl;

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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

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
         parser.read("ac") &&
         (parser.result() == true);

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail: " << parser.result() << std::endl;

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
         a and not b or (c and z);
         //And(a, _notb);

      success = success &&
         parser.read("cz") &&
         (parser.result() == true);

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail: " << parser.result() << std::endl;

      return success;
   }
}