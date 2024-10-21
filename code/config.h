#pragma once

#include "Config.hpp"

#if __has_include("../config.h")
   #include "../config.h"
#endif


#ifndef LOGON_TIMEOUT
   #define LOGON_TIMEOUT (60 * 60)
#endif

#ifndef BEE_FISH_SERVER_LOG_FILE
   #define BEE_FISH_SERVER_LOG_FILE    HOME_DIRECTORY "/server.log"
#endif

#ifndef  BEE_FISH_SERVER_ERR_FILE
   #define BEE_FISH_SERVER_ERR_FILE    HOME_DIRECTORY "/server.err"
#endif

#ifndef BEE_FISH_DATABASE_FILE
   #define BEE_FISH_DATABASE_FILE      HOME_DIRECTORY "/bee.fish.data"
#endif

#ifndef BEE_FISH_TRANSACTION_FILE
   #define BEE_FISH_TRANSACTION_FILE   HOME_DIRECTORY "/bee.fish.transaction"
#endif
