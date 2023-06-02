#ifndef BEE_FISH__TEST_HPP
#define BEE_FISH__TEST_HPP

#include "Miscellaneous/Miscellaneous.hpp"
#include "DBServer.hpp"



namespace BeeFish
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;


   inline bool test()
   {
   
      bool success = true;

      cout << "Test DB Server" << endl;

      outputSuccess(success);

      return success;
   }
   

}

#endif
