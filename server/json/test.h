#ifndef BEE_FISH_JSON__TEST_H
#define BEE_FISH_JSON__TEST_H

#include "json.h"
#include "../parser/test.h"

using namespace std;
using namespace bee::fish::parser;

namespace bee::fish::json
{
   inline bool test();
   
   inline bool testBlankSpace();
   inline bool testNumbers();
   
   inline bool test()
   {
      bool ok = true;
      
      ok &= testBlankSpace();
      ok &= testNumbers();
      
      if (ok)
         cout << "SUCCESS";
      else
         cout << "FAIL";
         
      cout << endl;
      
      return ok;
   }
   
   inline bool testBlankSpace()
   {
      bool ok = true;
      
      And sentence = And(
         new Word("Hello"),
         new BlankSpace(),
         new Word("World")
      );
         
      And match = sentence;
      ok &= testMatch("Blank space match", match, "Hello   World", true, "Hello   World");
      
      And noMatch = sentence;
      ok &= testMatch("Blank space no match", noMatch, "HelloWorld");

      return ok;
   }
   
   inline bool testNumbers()
   {
      bool ok = true;
      
      ok &= testMatch("Number::Integer", *Ptr<Number::Integer>(), "1234", true, "1234");
      ok &= testMatch("Number::Fraction", *Ptr<Number::Fraction>(), ".999", true, ".999");
      ok &= testMatch("Number::Exponent", *Ptr<Number::Exponent>(), "e+10", true, "e+10");
      ok &= testMatch("Number integer", *Ptr<Number>(), "1234", true, "1234");
      ok &= testMatch("Number negative number", *Ptr<Number>(), "-99.99", true, "-99.99");
      ok &= testMatch("Number fraction", *Ptr<Number>(), "999.99", true, "999.99");
      ok &= testMatch("Number exponent", *Ptr<Number>(), "1.234e-100", true, "1.234e-100");
      ok &= testMatch("Number unsigned exponent", *Ptr<Number>(), "1.234E100", true, "1.234E100");
      Number negative;
      ok &= testMatch("Number negative test", negative, "-888", true, "-888");
      cout << "Number negative sign value:\t";
      if (negative._sign->_value == "-")
         cout << "ok" << endl;
      else
         cout << "FAIL" << endl;
      
      return ok;
   }
   

         


   

   

}

#endif
