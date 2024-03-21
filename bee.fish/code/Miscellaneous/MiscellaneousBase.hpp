#ifndef BEE_FISH__MISCELLANEOUS_BASE_HPP
#define BEE_FISH__MISCELLANEOUS_BASE_HPP

#include <string>
#include <unistd.h>
#include <memory>
#include <filesystem>
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

   
   inline void sleep(long seconds) {
      usleep(seconds * 1000L * 1000L);
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
      
      switch((int)c) {
         case '\r':
            return "\\r";
         case '\n':
            return "\\n";
         case '\t':
            return "\\t";
         case '\\':
            return "\\\\";
         case '\"':
             return "\\\"";
         case -1:
             return "{eof}";
         default: {
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
      std::string output;
      for (const char& c : input) {

         output += escape(c);

      }

      return output;
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
   
   
}


   
#endif