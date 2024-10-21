#ifndef BEE_FISH__MISCELLANEOUS_BASE_HPP
#define BEE_FISH__MISCELLANEOUS_BASE_HPP

#include <string>
#include <unistd.h>
#include <memory>
#include <filesystem>
#include <pwd.h>
#include <cmath>
#include "Log.hpp"
#include "Optional.hpp"
#include "../Config.hpp"

#define ON_SUCCESS "😃"
#define ON_FAIL "🚫"


namespace BeeFishMisc {

   using namespace std::filesystem;
   
   inline int hasArg(
      int argc,
      const char* argv[],
      const std::string& arg
   )
   {
      for (int i = 0; i < argc; i++)
      {
         if (arg == argv[i])
            return i;
      }
   
      return -1;
   }

   
   inline void sleep(double seconds) {
      usleep((unsigned long)ceil(seconds * 1000.0 * 1000.0));
   }

   inline void outputSuccess(bool success)
   {
      if (success)
         std::cout << ON_SUCCESS << std::endl;
      else
         std::cout << ON_FAIL << std::endl;
   }

   inline std::string escape(
      char c
   )
   {
      switch ((int)c)
      {
      case 0:
         return "{null}";
         break;
      case '\"':
         return "\\\"";
         break;
      case '\\':
         return "\\\\";
         break;
      case '\b':
         return "\\b";
         break;
      case '\f':
         return "\\f";
         break;
      case '\r':
         return "\\r";
         break;
      case '\n':
         return "\\n";
         break;
      case '\t':
         return "\\t";
         break;
      case -1:
         return "{eof}";
         break;
      default:
         {
            std::string str;
            str = c;
            return str;
         }
      }
      
      return "";
   }

   inline std::string escape(
      const std::string& input
   )
   {
      std::stringstream stream;
      for (char c : input) {

         stream << escape(c);

      }

      return stream.str();
   }
   
   inline size_t getPageSize() {
      return PAGE_SIZE;
   }
   
   inline std::string now() {
      time_t t = time(0);
      char buffer[9] = {0};

      strftime(buffer, 9, "%H:%M:%S", localtime(&t));
      return std::string(buffer);
   }

   inline bool compareFiles(std::string file1, std::string file2)
   {
      // Compare the files
      std::stringstream stream;
      stream  << "diff -s -Z "
              << file1
              << " "
              << file2;
                
      std::string command = stream.str();
         
      return
         (system(command.c_str()) == 0);
      
   }
   
   inline std::string getUserName()
   {
      uid_t uid = geteuid ();
      struct passwd *pw = getpwuid (uid);
      if (pw)
      {
         return std::string(pw->pw_name);
      }
      return {};
   }

   
}


   
#endif