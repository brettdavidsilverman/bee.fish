#ifndef BEE_FISH_TEST__TEST_H
#define BEE_FISH_TEST__TEST_H

#include <fstream>
#ifdef SERVER
#include <filesystem>
#endif

#include "../parser/parser.h"
#include "../b-string/b-string.h"
#include "../json/json-parser.h"

#include "test-result.h"

#include "../Miscellaneous/Miscellaneous.hpp"

namespace BeeFishTest
{

#ifdef SERVER
   using namespace std::filesystem;
#endif

   using namespace BeeFishParser;
   using namespace BeeFishMisc;

#ifdef SERVER
   inline bool testFile(
      BeeFishJSON::JSONParser& parser,
      string label,
      path file,
      BeeFishParser::Match& match,
      optional<bool> expectedResult
   )
   {
      bool ok = true;
      
      cout << "Opening file " << file << endl;
      
     // file = string(HOME_DIR) + "/" + string(file);

      if (!exists(file)) {
         cout << "File not found: " << file << endl;
         return false;
      }

      // open the sample session file
      ifstream input(file);
      parser.read(input);
      
      if (expectedResult != nullopt)
         parser.eof();
      
      ok &= testResult(
         label,
         (match.result() == expectedResult)
      );

      if (!ok) {
         cout << "Expected: " << expectedResult << endl;
         cout << "Got: " << match.result() << endl;
      }
      
      input.close();
      
      return ok;
   }

   inline bool testFile(
      string label,
      path file,
      BeeFishParser::Match& match,
      optional<bool> result
   )
   {
      BeeFishJSON::JSONParser parser(match);
      return testFile(parser, label, file, match, result);
   }
#endif

   inline bool testMatch(
      Parser& parser,
      BString label,
      Match* match,
      string text,
      optional<bool> expectedResult = false,
      BString expected = {}
   )
   {
      cout << '\t' << label << ":\t" << flush;
      
      bool ok = true;

      parser.read(text);
      
      if (expectedResult != nullopt)
      {
         parser.eof();
      }
      
      BString value;

      if (match->matched())
         value = match->value();

      ok = (match->result() == expectedResult);

      if (expected.size())
      {
         if (value != expected)
            ok = false;
      }
      
      BeeFishMisc::outputSuccess(ok);
      
      if (!ok)
      {
         cout << "FAIL. Expected "
              << expectedResult
              << " Got "
              << match->result();
              
         if (parser.getError().length())
             cout
                 << " With error "
                 << parser.getError();
              
         cout << endl;

         cout << "\tTested   '" << text << "'" << endl;
         cout << "\tExpected '" << expected << "'" << endl;
         cout << "\tCaptured '" << value << "'" << endl;
      }
      
      return ok;
   }
   
   inline bool testMatch(
      BString label,
      Match* match,
      string text,
      optional<bool> result = false,
      BString expected = {}
   ) 
   {
      BeeFishJSON::JSONParser parser(match);
      //Parser parser(match);
      return testMatch(parser, label, match, text, result, expected);
   }

   inline bool testMatchDelete(
      BString label,
      Match* match,
      string text,
      optional<bool> result = false,
      BString expected = {}
   )
   {
      bool ok = testMatch(label, match, text, result, expected);
      delete match;
      return ok;
   }
   
   inline bool testValue(
      string expected,
      string value
   )
   {
      bool success = (expected == value);
      cout << "\t" << escape(expected) << ":{" << escape(value) << "}" << flush;
     
      BeeFishMisc::outputSuccess(success);

      return success;
   }
   
   inline bool testValue(
      string label,
      bool value
   )
   {
      bool success = (value);
      cout << "\t" << label << ": " << flush;
     
      BeeFishMisc::outputSuccess(success);

      return success;
   }
}

#endif