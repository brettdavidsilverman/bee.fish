#include "Parser.hpp"

namespace BeeFishParser {

   bool testUnicode();
   bool testCharacter();
   bool testReadCharacter();

   inline bool test() {
      
      cout << "Testing parser" << endl << endl;

      Parser parser;
      if (!parser.read(""))
         return false;

      if (!testUnicode())
         return false;

      if (!testCharacter())
         return false;

      if (!testReadCharacter())
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

      UTF8Character a("a");
      Character character(a);

      std::cout << character << std::flush;

      success = success && 
         (character.read("a") == true);

      if (success)
         std::cout << "😃" << std::endl;
      else
         std::cout << "Fail" << std::endl;

      return success;
      
   }
}