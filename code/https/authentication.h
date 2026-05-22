#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H

#include <exception>
#include <ctime>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Database/Database.hpp"
#include "../Authentication/authentication.h"

using namespace BeeFishDatabase;
using namespace BeeFishPowerEncoding;
using namespace BeeFishHTTPS;
using namespace BeeFishWeb;
using namespace BeeFishMisc;

namespace BeeFishHTTPS {



    class Authentication :
        public BeeFishAuthentication::Authentication
    {
  
    public:
        // Defined in session.h
        Authentication(
            Session* session
        );
    
        
    };
    
    

};

#endif