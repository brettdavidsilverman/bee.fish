#ifndef MOD_DATABASE_HPP
#define MOD_DATABASE_HPP

#include <unistd.h>
#include <syslog.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <new>

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

#include "Database/Database.hpp"
#include "parser/parser.h"
#include "ApacheStream.hpp"
#include "Miscellaneous/Debug.hpp"
#include "../Config.hpp"

static Debug debug;
static Database database(DATABASE_FILENAME);

namespace BeeFishWebServer {
    
   using namespace BeeFishDatabase;
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;

   using namespace std;

   static int counter = 0;

   static bool inputJSON(Path path, request_rec *r);
   static bool outputJSON(Path path, request_rec *r);
   static bool outputDocument(Path path, request_rec *r);
   
   
}


#endif