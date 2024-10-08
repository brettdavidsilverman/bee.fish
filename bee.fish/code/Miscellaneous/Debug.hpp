#ifndef BEE_FISH_MISCELLANEOUS__DEBUG
#define BEE_FISH_MISCELLANEOUS__DEBUG

#include <fstream>
#include "MiscellaneousBase.hpp"
#include "../Config.hpp"


namespace BeeFishMisc {

   using namespace std;
   
   class Debug : public ofstream
   {
   public:
      Debug()
      {
         open();
      }
      
      virtual ~Debug()
      {
         flush();
         close();
      }
      
      virtual void open()
      {
         ofstream::open(
            DEBUG_FILE,
            std::ios_base::app
         );
      }
      
      void timestamp() {
         (*this) << now();
      }
      
   private:
      
   };

}

#endif