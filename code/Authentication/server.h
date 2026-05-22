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
            
        if (_port != 443)
            stream << ":" << _port;
                
        _origin = stream.str();
        
        std::cout << "Setting up database..." << std::endl;

        for (Size i = 0; i < _databaseCount ; ++i)
        {
            _databases.push_back(
                new JSONDatabase(_databaseFile)
            );
        }

    }

    virtual ~Server()
    {
        for (Index i = 0; i < _databaseCount; ++i)
            delete _databases[i];

    }

    const BString& origin() const
    {

        return _origin;
    }

    const BString& databaseFile() const
    {
        return _databaseFile;
    }

    const unsigned short port() const
    {

        return _port;
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
    BString _origin;
    unsigned short _port;
    BString _databaseFile;
    Size _databaseCount;
    std::vector<JSONDatabase*> _databases;
    std::vector<std::mutex> _databaseLocks;
    std::mutex _databaseLock;
};




}

#endif