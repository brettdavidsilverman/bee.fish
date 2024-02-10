#ifndef BEE_FISH__TEST_HPP
#define BEE_FISH__TEST_HPP

#include <string>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"

namespace BeeFishTest {

   using namespace std;
   using namespace BeeFishMisc;

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
   
   inline bool testPattern(
      const std::string& label,
      BeeFishParser::Parser&
         parser,
      const std::string& pattern,
      const std::optional<bool>&
          expected
   )
   {
      using namespace std;
      using namespace BeeFishMisc;
      using namespace BeeFishParser;

      bool success = true;
    
      cout << "\t" << label << ":" << flush;
      
      success =
         parser.read(pattern);

      if (success &&
          parser._result == nullopt)
         parser.eof();
      
      cout << "{"
           << parser.result()
           << "}";

      success =
        ( parser.result() == expected );

      BeeFishMisc::outputSuccess(success);

      return success;
   }
   

   inline bool testPattern(
      BeeFishParser::Parser&
         parser,
      const std::string& pattern,
      const std::optional<bool>&
          expected
   )
   {
      
       return testPattern(
          escape(pattern),
          parser,
          pattern,
          expected
       );

   }

   inline bool testPattern(
      const BeeFishParser::Parser&
         parser,
      const std::string& pattern,
      const std::optional<bool>&
          expected
   )
   {
      using namespace BeeFishParser;

      unique_ptr<Parser> copy =
         unique_ptr<Parser>(
            parser.copy()
         );

      bool success = testPattern(
         *copy,
         pattern,
         expected
      );

      return success;
   }
   
   
}

#endif
