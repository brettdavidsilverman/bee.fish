#ifndef BEE_FISH__MISCELLANEOUS__LOG_HPP
#define BEE_FISH__MISCELLANEOUS__LOG_HPP

#include <syslog.h>
#include <unistd.h>
#include <stdarg.h>

// Should log to /var/log/messages

namespace BeeFishMisc {

   void openLog() {

      setlogmask (LOG_UPTO (LOG_NOTICE));

      openlog (NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

      syslog (LOG_NOTICE, "Program started by User %d", getuid ());

   }

   template<typename ... Args>
   void logMessage(int priority, const std::string& message, Args ... args)
   {

      fprintf(stderr, message.c_str(), args ...);
      
      syslog(priority, message.c_str(), args ...);
      
   }

   void closeLog() {
      closelog();
   }

}


#endif