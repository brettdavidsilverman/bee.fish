#ifndef BEE_FISH__DATABASE__TEST_HPP
#define BEE_FISH__DATABASE__TEST_HPP
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Parser/Test.hpp"
#include "Database.hpp"



namespace BeeFishDatabase
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;


   inline bool test()
   {
   
      bool success = true;

      cout << "Test Database" << endl;

      Database db("temp.data");

      Path start(db);
      Path next = start["Hello"];

      next.setData("world");

      std::string world;
      start["Hello"].getData(world);

      success &=
         testValue(
            "world",
            world
         );

      outputSuccess(success);

      return success;
   }
   

}

#endif
