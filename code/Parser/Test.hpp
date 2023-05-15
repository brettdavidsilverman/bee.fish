#include "Parser.hpp"
#include "Character.hpp"
#include "And.hpp"
#include "Or.hpp"

namespace BeeFishParser {

   bool testUnicode();
   bool testCharacter();
   bool testReadCharacter();
   bool testAnd();
   bool testOr();

   inline bool test() {
      
      std::cout << "Testing parser" << std::endl << std::endl;

      Parser parser;
      if (!parser.read(""))
         return false;

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

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

      return success;

      
   }

   inline bool testReadCharacter() {

      bool success = true;

      std::cout << "testReadCharacter: " << std::flush;

      UTF8Character character("a");

      std::string stream("a");

      success = success && 
         character.read(stream) &&
         character._result;

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

      return success;
      
   }

   inline bool testAnd() {

      bool success = true;

      std::cout << "testAnd: " << std::flush;

      Character character1("a");
      Character character2("b");

      And _and(character1, character2);

      std::string stream("ab");

      success = success &&
         _and.read(stream) &&
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
}