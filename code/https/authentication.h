#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H

#include <exception>
#include <ctime>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Database/Database.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishPowerEncoding;
using namespace BeeFishHTTPS;
using namespace BeeFishWeb;
using namespace BeeFishMisc;

namespace BeeFishHTTPS {

   class Session;
   
   class Authentication
   {
   private:
      JSONDatabase& _database;
      bool _authenticated = false;
      Path _path;
      Path _sessionData;
      Path _userData;
      
   protected:
      BString _ipAddress;
      BString _sessionId;
      
   public:
      
      static time_t epoch_seconds()
      {
         std::time_t result = std::time(nullptr);
         std::localtime(&result);
         return result;
      }
      
   
      inline static const size_t
         SESSION_ID_SIZE = 32;
  
   public:
      // Implemented in session.h
      Authentication(Session* session);
      
      Authentication(
         JSONDatabase& database,
         BString ipAddress,
         BString sessionId
      ) :
         _database(database),
         _path(_database.origin()),
         _sessionData(_path),
         _userData(_path),
         _ipAddress(ipAddress),
         _sessionId(sessionId)
      {
         _authenticated = false;

         if (_sessionId.size())
            authenticate();
      }
     
   public:
      virtual void logon(const BString& secret)
      {
          

         if (!_ipAddress.size())
            throw runtime_error("Missing ip-address");
            
         if (!secret.size())
            throw runtime_error("Missing secret");
                        
         _authenticated = false;

         
         // Save the secret
         // and set the user data path
         BString md5Secret =
            md5(secret);

         _userData = _path
            ["Secrets"]
            [md5Secret];
                  
         // Create the session id
         // (Note, we use toHex, not toBase64 due to
         // cookie encoding rules)
         _sessionId =
            toHex(
               createRandom(
                  SESSION_ID_SIZE
               )
            );

         // get the session data
         _sessionData = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId];
               

         // Save the secret path
         _sessionData["User Data Path"]
            .setData<Index>(
               _userData._index
            );

            
         // Save last authenticated
         time_t lastAuthentication = epoch_seconds();

         _sessionData["Last Authentication"]
            .setData<time_t>(lastAuthentication);
            
         _authenticated = true;

      }
      
      virtual void logoff()
      {
         if (_authenticated)
         {
            _sessionData.clear();
         }
         
         _sessionId.clear();
         _authenticated = false;

      }
      
   protected:

      virtual bool authenticate() {

         _authenticated = false;

         if ( _ipAddress.size() &&
              _sessionId.size() )
         {

            _sessionData = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId];
               
            if ( _sessionData
                    ["Last Authentication"]
                    .hasData() )
            {
          
               time_t lastTime = -1;
               
               _sessionData
                  ["Last Authentication"]
                  .getData<time_t>(lastTime);
                  
               // 1 hour duration
               if ( (epoch_seconds() - lastTime) 
                    < LOGON_TIMEOUT )
               {
                  _authenticated = true;
               
                  _sessionData["User Data Path"]
                     .getData<Index>(
                        _userData._index
                     );
                     
                  _sessionData
                     ["Last Authentication"]
                     .setData<time_t>(epoch_seconds());
               }
            }
         }

         return _authenticated;

      }

   public:
   
      BeeFishDatabase::Path userData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return _userData;
      }
      
      BeeFishDatabase::Path sessionData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return _sessionData;
      }
      
      virtual void write(BeeFishScript::Object& object) const {
         object["authenticated"] = _authenticated;
         if (_authenticated)
         {
            object["timeout"] = 
               BeeFishScript::Integer(LOGON_TIMEOUT);
            object["sessionId"] = sessionId();
         }
         else
            object["timeout"] =
               BeeFishScript::Integer(0);
      }
      
      operator bool()
      {
         return _authenticated;
      }
      
      bool authenticated()
      {
         return _authenticated;
      }
      
      const BString& sessionId() const
      {
         return _sessionId;
      }

      BString& sessionId()
      {
         return _sessionId;
      }

   };
   
   

};

#endif