#ifndef BEE_FISH_DATABASE__DATABASE_BASE_HPP
#define BEE_FISH_DATABASE__DATABASE_BASE_HPP

#include "Config.hpp"

#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <filesystem>
#include <queue>
#include <mutex>

#include "../Miscellaneous/Debug.hpp"
#include "../b-string/b-string.h"
#include "Version.hpp"
#include "File.hpp"
#include "LockFile.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "LeastRecentlyUsedCache.hpp"

using namespace std;
using namespace std::literals;



namespace BeeFishDatabase {
    
    using namespace BeeFishMisc;
    
    // Store [left, right] branch elements.
    // A zero is stored if the branch
    // hasnt been visited yet.
    class Database : public LockFile
    {
    
    
    public:
        typedef PowerEncoding Encoding;

    public:

        Size _pageSize = 0;
        
    protected:
        LeastRecentlyUsedCache<Index, Branch>
            _cache;
    public:

        struct ScopedFileLock{

            Database* _database;

            ScopedFileLock(Database* database) {
                _database = database;
                _database->lock();
            }
            
            ~ScopedFileLock() {
                _database->unlock();
            }

        };

    public:
        Database(
            string filePath = "",
            const Size pageSize = getPageSize(),
            const Size cacheCapacity = 10000
        ) :
            LockFile(
                filePath
            ),
            _pageSize(pageSize),
            _cache(cacheCapacity)
        {
#ifdef DEBUG_
            Debug debug;
            debug << now() << " "
                    << "Database(\""
                    << escape(_filename)
                    << "\")"
                    << "\r\n";#endif
        }

        virtual ~Database()
        {
#ifdef DEBUG_
            Debug debug;

            debug << now() << " "
                    << "~Database(\""
                    << escape(_filename)
                    << "\")"
                    << "\r\n";
#endif
        }
        
    public:

        Size pageSize() const {
            return _pageSize;
        }


        inline Index getNextIndex(Index parent)
        {

            ScopedFileLock lock(this);

            Branch branch;
            branch._parent = parent;
            Index index = size();

            if (index == 0) {
                index = sizeof(Branch);
            }

            seek(index);

            write(&branch, sizeof(branch));

            return index;
        }
 
        inline Index allocate(const std::string& data)
        {

            
            ScopedFileLock lock(this);

            Index dataIndex = size();

            Size dataSize = data.size();
            write(&dataSize, sizeof(Size));
            write(data.data(), dataSize);


            return dataIndex;
                
        }
        
        inline Branch getBranch(Index index)
        {
            
            if (size() == 0) 
            {
                ScopedFileLock lock(this);
                if (size() == 0) {
                    Branch branch;
                    seek(0);
                    write(&branch, sizeof(Branch));
                }
            }

            optional<Branch> branch =
                _cache.get(index);
                
            if (branch == nullopt)
            {
                Branch branch;
                seek(index);
                read(&branch, sizeof(Branch));
                _cache.put(index, branch);
                return branch;
            }

            
            return branch.value();
        }


        inline void setBranch(Index index, const Branch& branch)
        {

            seek(index);
            write(&branch, sizeof(Branch));
            _cache.put(index, branch);
            
        }
        

        // Waits till lock on branch is released
        // Sets the lock on the branch to true
        // Returns the latest version of the branch.
        inline Branch lockBranch(Index lockIndex) 
        {

            lock();

            Branch branch = getBranch(lockIndex);

            // Wait until branch is unlocked
            while (branch._locked) {
                // Unlock the process lock
                unlock();
                
                // Wait until branch is unlocked
                while (branch._locked) {

                    // Sleep to avoid spin wait
                    BeeFishMisc::sleep(0.01);

                    // Refresh the branch
                    branch = getBranch(lockIndex);

                }

                // Regain exclusive lock
                lock();

                // Get the refreshed branch
                branch = getBranch(lockIndex);
            }

            // Because we have an exclusive lock
            // and have rereshed the branch
            // The branch must have been unlocked
            // by another process
    
            // Set the branch locked flag
            branch._locked = true;
            setBranch(lockIndex, branch);

            // Release the process lock
            unlock();
            
            // Return the lated version of branch
            return branch;
        }

        inline void unlockBranch(Index lockIndex) 
        {
            ScopedFileLock lock(this);

            Branch branch = getBranch(lockIndex);

            if (branch._locked) {
                branch._locked = false;
                setBranch(lockIndex, branch);
            }
        }
        
        inline std::string getData(Index dataIndex)
        {
            if (dataIndex == 0)
                return "";


            seek(dataIndex);
            Size size;

            read(&size, sizeof(Size));
            std::string buffer(size, '\0');
            read(buffer.data(), size);

            return buffer;
        }

        
        inline void setData(Index dataIndex, const std::string& source)
        {
            seek(dataIndex);
            Size size = source.size();
            write(&size, sizeof(Size));
            write(source.data(), size);
            
        }
 
        
        string version() const {
            return DATABASE_VERSION;
        }

        
        virtual void stripe(ostream& out) {

            std::queue<Index> queue;
            queue.push(0);

            while (queue.size()) 
            {
                Index index = queue.front();
                queue.pop();

                Branch branch = getBranch(index);

                out << index << '\t'
                     << branch._left << '\t'
                     << branch._right
                     << endl;

                if (branch._left)
                    queue.push(branch._left);

                if (branch._right)
                    queue.push(branch._right);

            }
        }
        
        virtual void unlockAll() {

            std::queue<Index> queue;
            queue.push(0);

            while (queue.size()) 
            {
                Index index = queue.front();
                queue.pop();

                Branch branch = getBranch(index);

                if (branch._locked)
                {
                    cout << "Unlocking " << index << endl;
                    branch._locked = false;
                    setBranch(index, branch);
                    
                }

                if (branch._left)
                    queue.push(branch._left);

                if (branch._right)
                    queue.push(branch._right);

            }
        }

        friend ostream& operator <<
        (ostream& out, const Database& db)
        {

            out << "Database: "
                 << db.version() << endl
                 << "Filename: "
                 << db._filename << endl
                 << "Branch size: "
                 << sizeof(Branch) << endl
                 << "Size: "
                 << db.size() << endl;
             
            return out;
        }

    };

}

#endif