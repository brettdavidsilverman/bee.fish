#ifndef BEE_FISH__MISCELLANEOUS_BASE_HPP
#define BEE_FISH__MISCELLANEOUS_BASE_HPP

#include <string>
#include <unistd.h>
#include <memory>
#include <filesystem>
#include <pwd.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cassert>
#include "Log.hpp"
#include "Optional.hpp"
#include "../Size.hpp"
#include "../Config.hpp"

#define ON_SUCCESS "😃"
#define ON_FAIL "🚫"


namespace BeeFishMisc {

    using namespace std::filesystem;
    using namespace std;
    
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
            return "\0";
        case '\"':
            return "\\\"";
        case '\\':
            return "\\\\";
        case '\b':
            return "\\b";
        case '\f':
            return "\\f";
        case '\r':
            return "\\r";
        case '\n':
            return "\\n";
        case '\t':
            return "\\t";
        case -1:
            return "{eof}";
        
        }
        
        
        std::string str;
        str = c;
        return str;
    
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
    
    inline char unescape(
        char c
    )
    {
        switch (c)
        {
        case '0':
            return '\0';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case '\b':
            return '\b';
        case '\f':
            return '\f';
        case '\r':
            return '\r';
        case '\n':
            return '\n';
        case '\t':
            return '\t';
        case -1:
            return (char)-1;
        }
        
        return c;
        
    }

    inline std::string unescape(
        const std::string& input
    )
    {
        std::stringstream stream;
        for (Size i = 0; i < input.size(); ++i) {
            char c = input[i];
            if (c == '\\' && i < input.size()) {
                char c2 = input[++i];
                stream << unescape(c2);
            }
            else
                stream << c;

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

    inline bool compareFiles(const std::string& p1, const std::string& p2, bool display = true) {
        std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
        std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

        bool equal = true;
        
        if (f1.fail() || f2.fail()) {
            // Handle file open errors
            std::cerr << "Error opening files." << std::endl;
            return false;
        }

        // 1. Check file sizes
        if (f1.tellg() != f2.tellg()) {
            equal = false; // Files have different sizes, so they are different
        }

        if (equal)
        {
             // Return to the beginning of the files
            f1.seekg(0, std::ifstream::beg);
            f2.seekg(0, std::ifstream::beg);
        }
        
        // 2. Compare file contents in chunks
        // You can use a specific buffer size, or simply compare using iterators for simplicity
        if (equal)
        {
            equal = std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
        }
        
        if (!equal && display) {
            f1.seekg(0, std::ifstream::beg);
            f2.seekg(0, std::ifstream::beg);
            cout << p1 << endl;
            cout << f1.rdbuf() << endl;
            
            cout << "-----" << endl;
            
            cout << p2 << endl;
            cout << f2.rdbuf() << endl;
            
        }
        
        return equal;
        
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