#ifndef BEE_FISH_SERVER__LOG_H
#define BEE_FISH_SERVER__LOG_H
#include <fstream>
#include "../Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"

inline std::ofstream &logfile()
{
   static std::ofstream file(BEE_FISH_SERVER_LOG_FILE, std::ios::out | std::ios::app);
   return file;
}

inline std::ofstream &errfile()
{
   static std::ofstream file (BEE_FISH_SERVER_ERR_FILE, std::ios::out | std::ios::app);
   return file;
}

inline void initializeLogs() {

   using namespace std;
   using namespace BeeFishMisc;
   
#ifndef DEBUG
   clog.rdbuf(logfile().rdbuf());
   cerr.rdbuf(errfile().rdbuf());
#endif

   cout << now() << " cout output" << endl;
   cerr << now() << " cerr output" << endl;
   clog << now() << " clog output" << endl;

}
#endif