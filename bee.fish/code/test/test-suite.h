#ifndef BEE_FISH_TEST__TEST_SUITE
#define BEE_FISH_TEST__TEST_SUITE

#include "../b-string/string.h"
#include "../b-string/test.h"
#include "../parser/test.h"
#include "../json/test.h"
#include "../power-encoding/test.h"
#include "../id/test.h"
#include "../json/json-parser.h"
#include "../web-request/test.h"
#include "../web-response/test.h"
#include "../misc/debug.h"
#include "../b-script/test.h"
//#include "../web-server/web-server.h"

#ifdef SERVER
#include "../database/test.h"
#include "../https/test.h"
#endif


namespace BeeFishTest
{

   
   
   inline bool test()
   {
 
      cout << "****BeeFishBString" << endl;
      if (!BeeFishBString::test())
      {
         cout << "****B-String FAILED" << endl;
         return false;
      }
      
      cout << "****BeeFishParser" << endl;
      if (!BeeFishParser::test())
      {
         cout << "****Parser FAILED" << endl;
         return false;
      }
      
      cout << "****BeeFishJSON" << endl;
      if (!BeeFishJSON::test())
      {
         cout << "****JSON FAILED" << endl;
         return false;
      }
      
      cout << "****BeeFishWeb::testRequest" << endl;
      if (!BeeFishWeb::testRequest())
      {
         cout << "****WEB-REQUEST FAILED" << endl;
         return false;
      }

      cout << "****BeeFishWeb::testResponse" << endl;
      if (!BeeFishWeb::testResponse())
      {
         cout << "****WEB-RESPONSE FAILED" << endl;
         return false;
      }

      cout << "****BeeFishPowerEncoding" << endl;
      if (!BeeFishPowerEncoding::test())
      {
         cout << "****Power Encoding FAILED" << endl;
         return false;
      }

      cout << "****BeeFishId" << endl;
      if (!BeeFishId::test())
      {
         cout << "****ID FAILED" << endl;
         return false;
      }

#ifdef SERVER      

      cout << "****BeeFishDatabase" << endl;
      if (!BeeFishDatabase::test())
      {
         cout << "****Database FAILED" << endl;
         return false;
      }
      
      cout << "****BeeFishHTTPS" << endl;
      if (!BeeFishHTTPS::test())
      {
         cout << "****HTTPS FAILED" << endl;
         return false;
      }
#endif

      cout << "****BeeFishBScript" << endl;
      if (!BeeFishBScript::test()) 
      {
         cout << "****B-Sript FAILED" << endl;
         return false;
      }

      return true;
      
   }
   
   
   
}

#endif