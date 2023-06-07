#ifndef BEE_FISH__MISCELLANEOUS__LOG_HPP
#define BEE_FISH__MISCELLANEOUS__LOG_HPP

#include <syslog.h>
#include <unistd.h>

// Should log to /var/log/messages

namespace BeeFishMisc {

   void openLog() {

      setlogmask (LOG_UPTO (LOG_NOTICE));

      openlog (NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

      syslog (LOG_NOTICE, "Program started by User %d", getuid ());

   }

   void log(int priority, const std::string& message)
   {
   
      syslog (priority, message.c_str());

   }

   void closeLog() {
      closelog();
   }

}


#endif