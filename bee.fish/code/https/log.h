#ifndef BEE_FISH_SERVER__LOG_H
#define BEE_FISH_SERVER__LOG_H
#include <fstream>
#include "../config.h"

inline std::ofstream &logfile()
{
   static std::ofstream lf(BEE_FISH_SERVER_LOG_FILE, std::ios::out | std::ios::app);
   return lf;
}

inline std::ofstream &errfile()
{
   static std::ofstream lf (BEE_FISH_SERVER_ERR_FILE, std::ios::out | std::ios::app);
   return lf;
}

inline void initializeLogs() {

   using namespace std;
   
#ifndef DEBUG   
   clog.rdbuf(logfile().rdbuf());
   cerr.rdbuf(errfile().rdbuf());
#endif

   cout << "cout output" << endl;
   cerr << "cerr output" << endl;
   clog << "clog output" << endl;

}
#endif