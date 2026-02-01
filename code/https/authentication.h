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
        
        bool _authenticated = false;
        
    protected:
        Session* _session;
        BString _origin;
        BString _ipAddress;
        BString _sessionId;
        BString _userId;
        
    public:
        
        static time_t epoch_seconds()
        {
            std::time_t result = std::time(nullptr);
            std::localtime(&result);
            return result;
        }
        
    
        inline static const size_t
            SESSION_ID_SIZE = 32;
            
        inline static const size_t
            USER_ID_SIZE = 32;
  
    public:
        // Implemented in session.h
        Authentication(Session* session);
        
        
        
        Authentication(
            Session* session,
            const BString& origin,
            BString ipAddress,
            BString sessionId
        ) :
            _session(session),
            _origin(origin),
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
            BString md5Secret =
                md5(secret);

            ScopedDatabase scoped(this);
            JSONDatabase* database = scoped;
            
            Path rootPath = database->root();
            
            Path secrets = rootPath
                [SECRETS]
                [md5Secret];
                
            // Get or set the userId
            if (secrets.hasData())
                _userId = secrets.getData();
            else
            {
                _userId =
                    toHex(
                        createRandom(
                            USER_ID_SIZE
                        )
                    );
                    
                secrets.setData(_userId);
                
            }
            
            // Set the user data path
            Path userData = database->userData(_userId);
            
                
                        
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
            Path sessionData = 
                database->sessionData(_ipAddress, _sessionId);
                
            
            // Save the user id
            sessionData[USER_ID]
                .setData(_userId);

                
            // Save last authenticated
            time_t lastAuthentication = epoch_seconds();

            sessionData[LAST_AUTHENTICATION]
                .setData<time_t>(lastAuthentication);
                
            _authenticated = true;
            

        }
        
        virtual void logoff()
        {
            if (_authenticated)
            {
                ScopedDatabase scoped(this);
                JSONDatabase* database = scoped;
                Path rootPath = database->root();
                Path sessionData =
                    database->sessionData(
                        _ipAddress,
                        _sessionId
                    );
                    
                sessionData.clear();
            }
            
            _sessionId.clear();
            _userId.clear();
            
            _authenticated = false;

        }
        
    protected:

        virtual bool authenticate() {

            _authenticated = false;

            ScopedDatabase scoped(this);
            JSONDatabase* database =
                scoped;
                
            Path rootPath = database->root();
                
            if ( _ipAddress.size() &&
                  _sessionId.size() )
            {

                Path sessionData =
                    database->sessionData(
                        _ipAddress,
                        _sessionId
                    );
                    
                if ( sessionData
                          [LAST_AUTHENTICATION]
                          .hasData() )
                {
             
                    time_t lastTime = -1;
                    
                    sessionData
                        [LAST_AUTHENTICATION]
                        .getData<time_t>(lastTime);
                        
                    // 1 hour duration
                    if ( (epoch_seconds() - lastTime) 
                          < LOGON_TIMEOUT )
                    {
                        _authenticated = true;
                    
                        sessionData
                            [LAST_AUTHENTICATION]
                            .setData<time_t>(epoch_seconds());
                            
                        _userId =
                            sessionData[USER_ID]
                            .getData();
                
                        // Set the user data path
                        Path userData =
                            database->userData(_userId);
                        
                    }
                }
            }

            return _authenticated;

        }

    public:
    
        // Defined in session.h
        Server* server();
        
        virtual void write(BeeFishScript::Object& object) const {
            object["authenticated"] = _authenticated;
            if (_authenticated)
            {
                object["timeout"] = 
                    BeeFishScript::Integer(LOGON_TIMEOUT);
                object["sessionId"] = sessionId();
                object["userId"] = userId();
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
        
        const BString& userId() const
        {
            return _userId;
        }

        class ScopedDatabase
        {
        private:
            Authentication* _authentication;
            JSONDatabase* _database;
        public:
            ScopedDatabase(Authentication* authentication) :
                _authentication(authentication)
            {
                _database =
                    _authentication->server()->requestDatabase();
            }
            
            ~ScopedDatabase()
            {
                _authentication->server()->releaseDatabase(_database);
            }
            
            operator JSONDatabase*() {
                return _database;
            }
        };
        
    };
    
    

};

#endif