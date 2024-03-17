#ifndef BEE_FISH_MISC__DEBUG
#define BEE_FISH_MISC__DEBUG

#include <fstream>
#include "Miscellaneous.hpp"
#include "../Config.hpp"

using namespace std;

namespace BeeFishMisc {

   class Debug : public ofstream
   {
   public:
      Debug()
      {
         open();
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