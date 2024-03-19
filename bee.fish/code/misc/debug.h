#ifndef BEE_FISH_MISC__DEBUG
#define BEE_FISH_MISC__DEBUG

#include <stdexcept>
#include "../b-string/string.h"

#ifndef ESP32
#define ESP_OK (0)
#define ESP_FAIL (-1)
typedef int esp_err_t;
extern "C" inline const char* esp_err_to_name(esp_err_t err) {
    static const char * name = "";
    return name;
} 
#endif

#ifdef DEBUG
   #define DEBUG_OUT(x) \
        { std::cerr << x; }
#else
   #define DEBUG_OUT(x)
#endif

#define USE_FILTER

#if defined(DEBUG) && !defined(USE_FILTER)
    #define filterTag(result, tag) (true)
#else
    inline bool filterTag(int result, BString tag) {
        if (tag == "WebRequest" && result == 0)
            return false;
        else if (tag == "NeoPixels" && result == 0)
            return false;
        else
            return true;
    }
#endif


#define _INFO(result, tag, error, ...) {\
    if (filterTag(result, tag)) { \
        printf("%s: ", tag); \
        printf(error, ##__VA_ARGS__); \
        if (result != ESP_OK) \
            printf(" \"%s\"", esp_err_to_name(result)); \
        printf("\n"); \
    } \
}

#define INFO(tag, error, ...) {_INFO(ESP_OK, tag, error, ##__VA_ARGS__);}


#ifdef DEBUG
    #define ERROR(result, tag, error, ...) { \
        _INFO(result, tag, error, ##__VA_ARGS__); \
        throw std::runtime_error(tag); \
    }
#else
    #define ERROR(result, tag, error, ...) { \
        _INFO(result, tag, error, ##__VA_ARGS__); \
    }
#endif

#define CHECK_ERROR(result, tag, error, ...) { \
    if (result != ESP_OK) {\
        ERROR(result, tag, error, ##__VA_ARGS__);\
    } \
}


#endif