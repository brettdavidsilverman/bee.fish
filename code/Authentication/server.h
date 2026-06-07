#ifndef BEE_FISH_AUTHENTICATION__SERVER_H
#define BEE_FISH_AUTHENTICATION__SERVER_H

//
// server.h
// ~~~~~~~~~~
//
// Modified from
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <mutex>

#define SERVER

#include "../Config.hpp"

#include "../Database/Database.hpp"
#include "../b-string/b-string.h"
#include "../Miscellaneous/Date.hpp"


namespace BeeFishAuthentication {

using namespace BeeFishDatabase;
using namespace BeeFishWeb;

class Server
{
public:
    Server( const BString& hostName,
            unsigned short port,
            const BString& databaseFile,
            Index databaseCount
          ) :
        _port(port),
        _databaseFile(databaseFile),
        _databaseCount(databaseCount),
        _databaseLocks(_databaseCount)
    {
        
        
        stringstream stream;
        stream << "https://" + hostName;
            
        if (port != 443)
            stream << ":" << std::to_string(port);
                
        _origin = stream.str();
        
        setupDatabase();

    }
    
    Server( const BString& origin,
            const BString& databaseFile,
            Index databaseCount = 1
          ) :
        _origin(origin),
        _databaseFile(databaseFile),
        _databaseCount(databaseCount),
        _databaseLocks(_databaseCount)
    {
        URL url(origin);
        if (url.port().size())
            _port = atol(url.port().c_str());
        else
#ifdef DEBUG
            _port = 8000;
#else
            _port = 443;
#endif

        setupDatabase();
    }

    virtual ~Server()
    {
        for (Index i = 0; i < _databaseCount; ++i)
            delete _databases[i];
    }
    
    void setupDatabase() {

        for (Size i = 0; i < _databaseCount ; ++i)
        {
            _databases.push_back(
                new JSONDatabase(_databaseFile)
            );
        }
    }

    const BString& origin() const
    {

        return _origin;
    }

    const BString& databaseFile() const
    {
        return _databaseFile;
    }

    
    std::vector<JSONDatabase*>& databases()
    {
        return _databases;
    }

    JSONDatabase* requestDatabase()
    {
        while (1) {
            for (Size i = 0; i < _databaseCount; ++i)
            {
                if (_databaseLocks[i].try_lock())
                {
                    JSONDatabase*
                    database =
                        _databases[i];
                    return database;
                }
            }
            _databaseLock.lock();
        }
    }

    void releaseDatabase(JSONDatabase* database) {
        for (Index i = 0; i < _databaseCount; ++i)
        {
            if (_databases[i] == database) {
                _databaseLocks[i].unlock();
                _databaseLock.unlock();
                return;
            }
        }
    }


protected:
    unsigned short _port;
    BString _origin;
    BString _databaseFile;
    Size _databaseCount;
    std::vector<JSONDatabase*> _databases;
    std::vector<std::mutex> _databaseLocks;
    std::mutex _databaseLock;
};




}

#endif