#ifndef BEE_FISH__CONFIG_CODE_HPP
#define BEE_FISH__CONFIG_CODE_HPP

#include "../Config.hpp"

#define TEMP_DIRECTORY "/var/tmp/"
#define TEMP_FILENAME TEMP_DIRECTORY DOMAIN ".data"

#define DEBUG_FILE HOME_DIR "/debug.txt"

#ifndef LOGON_TIMEOUT
   #define LOGON_TIMEOUT (60 * 60)
#endif

#ifndef BEE_FISH_SERVER_LOG_FILE
   #define BEE_FISH_SERVER_LOG_FILE    DATA_DIR "/" DOMAIN ".log"
#endif

#ifndef  BEE_FISH_SERVER_ERR_FILE
   #define BEE_FISH_SERVER_ERR_FILE    DATA_DIR "/" DOMAIN ".err"
#endif

#ifndef BEE_FISH_DATABASE_FILE
   #define BEE_FISH_DATABASE_FILE      DATA_DIR "/" DOMAIN ".data"
#endif

#ifndef BEE_FISH_TRANSACTION_FILE
   #define BEE_FISH_TRANSACTION_FILE   DATA_DIR "/" DOMAIN ".transaction"
#endif

#ifndef CERT_FILE
   #define CERT_FILE "/etc/letsencrypt/live/" DOMAIN "/fullchain.pem"
#endif

#ifndef KEY_FILE
   #define KEY_FILE "/etc/letsencrypt/live/" DOMAIN "/privkey.pem"
#endif

#define TMP_DH_FILE WWW_ROOT_DIRECTORY "/dhparam4096.pem"

#endif