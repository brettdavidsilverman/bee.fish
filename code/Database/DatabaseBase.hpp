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
#include <boost/interprocess/managed_shared_memory.hpp>

#include "../Miscellaneous/Debug.hpp"
#include "../b-string/b-string.h"
#include "Version.hpp"
#include "File.hpp"
#include "LockFile.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "LeastRecentlyUsedCache.hpp"
#include "Allocator.hpp"

using namespace std;
using namespace std::literals;



namespace BeeFishDatabase {
    
   // using namespace BeeFishMisc;
    using namespace boost::interprocess;
            
    // Store [left, right] branch elements.
    // A zero is stored if the branch
    // hasnt been visited yet.
    class Database : public LockFile
    {
    
    
    public:
        typedef LeastRecentlyUsedCache<Index, Branch> Cache;
        Cache* _cache;
    public:
        
        
        Index _pageSize = 0;

    public:

        
        /*
        struct BranchLock {
            Index _index = 0;
            std::mutex _mutex;
            Branch _branch;
        };
        
        std::map<Index, Index> _branchMap;
        BranchLock* _branchLocks = nullptr;
        const char* _sharedMemoryName = "shared_memory";
        
        void* _sharedMemory = nullptr;
        shared_memory_object* _sharedMemoryObject = nullptr;
        mapped_region* _mappedRegion = nullptr;
        */
    public:
        Database(
            string filePath = "",
            const Index pageSize = getPageSize()
        ) :
            LockFile(
                filePath
            ),
            _pageSize(pageSize)
            
        {
            _cache = new Cache(*this, 100000);
        }

        virtual ~Database()
        {
            delete _cache;
                /*
            if (_sharedMemory)
               destroySharedMemory();
               
            */
            //unlock();
        }
        
    public:
        
        Cache& cache() {
            return *_cache;
        }

        Index pageSize() const {
            return _pageSize;
        }


        Index getNextIndex(Index parent)
        {

            ScopedFileLock lock(*this);

            Branch branch(parent);

            Index index = size();

            seek(index);

            write(&branch, sizeof(Branch));

            return index;
        }
 
        inline Index allocate(const std::string& data)
        {

            
            ScopedFileLock lock(*this);

            Index dataIndex = size();

            Index dataSize = data.size();
            write(&dataSize, sizeof(Index));
            write(data.data(), dataSize);


            return dataIndex;
                
        }

        void deleteBranch(Index index)
        {


            ScopedFileLock lock(*this);

            deleteData(index);
            
            Branch branch = getBranch(index);
            
            if (branch._left)
                deleteBranch(branch._left);
                    
            if (branch._right)
                deleteBranch(branch._right);
            
            if (index != rootIndex())
            {
                
            
                Branch parent = getBranch(
                    branch._parent
                );
            
                if (index == parent._left)
                {
                    parent._left = 0;
                }
                else if (index == parent._right)
                {
                    parent._right = 0;
                }
                else
                    assert(false);
                
                
                setBranch(branch._parent, parent);
            }
            
            // Reclaim space here
            setBranch(index, Branch());
        }
        
        void deleteData(Index index)
        {
            Database::ScopedFileLock lock(*this);

            Branch branch = getBranch(index);
            
            if (branch._dataIndex) {
                branch._dataIndex = 0;
                setBranch(index, branch);
            }
            
        }
        
        Branch getBranch(Index index)
        {
            ScopedFileLock lock(*this);
/*
            if (!_sharedMemory)
                createSharedMemory(_pageSize);
                */
            Branch branch;
            
            if (size() == 0) 
            {
                
                if (size() == 0)
                {
                    seek(0);
                    write(&branch, sizeof(Branch));
                }
            }

            optional<Branch> optionalBranch =
                cache().get(index);
                
            if (optionalBranch == nullopt)
            {
                seek(index);
                read(&branch, sizeof(Branch));
                cache().put(index, branch);
            }
            else
                branch = optionalBranch.value();
                
            
            
            return branch;


        }


        inline void setBranch(Index index, const Branch& branch)
        {
            ScopedFileLock lock(*this);

            seek(index);
            write(&branch, sizeof(Branch));
            cache().put(index, branch);
            
        }
        /*
        inline virtual void createSharedMemory(Index size)
        {
            
            try {
                
                _sharedMemoryObject = 
                    new shared_memory_object
                    (
                        create_only,
                        _sharedMemoryName,
                        read_write 
                    );
                    
                _sharedMemoryObject->truncate(size);
                
cerr << __FILE__ << " Created shared memory " << _pageSize << endl;
   
                //Map the whole shared memory in this process
                _mappedRegion = new mapped_region(*_sharedMemoryObject, read_write);

                _sharedMemory = _mappedRegion->get_address();
                
                //Write all the memory to 0
                std::memset(_sharedMemory, 0, _mappedRegion->get_size());
                
                BranchLock* branchLock = static_cast<BranchLock*>(_sharedMemory);

                for (Index i = 0; i < size / sizeof(BranchLock); ++i)
                {
                    new (branchLock++) BranchLock();
                }
            

                
                
      
            }
            catch (boost::interprocess::interprocess_exception& ex)
            {
                _sharedMemoryObject = 
                new shared_memory_object
                (
                    open_only,
                    _sharedMemoryName,
                    read_write 
                );
    
                //Map the whole shared memory in this process
                _mappedRegion = new mapped_region(*_sharedMemoryObject, read_write);
                

                _sharedMemory = _mappedRegion->get_address();

            
            }
            
            _branchLocks = static_cast<BranchLock*>(_sharedMemory);
            
        }
        
        inline void destroySharedMemory()
        {
            
            assert(_sharedMemory);
            delete _mappedRegion;
            delete _sharedMemoryObject;
            shared_memory_object::remove(_sharedMemoryName);
            _sharedMemory = nullptr;
            
        }
        */

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
            ScopedFileLock lock(*this);

            Branch branch = getBranch(lockIndex);

            if (branch._locked) {
                branch._locked = false;
                setBranch(lockIndex, branch);
            }
        }
        
        inline std::string getData(Index dataIndex)
        {
            ScopedFileLock lock(*this);
            
            if (dataIndex == 0)
                return "";


            seek(dataIndex);
            Index size;

            read(&size, sizeof(Index));
            std::string buffer(size, '\0');
            read(buffer.data(), size);

            return buffer;
        }

        inline bool hasData(Index index)
        {
            ScopedFileLock lock(*this);
            Branch branch = getBranch(index);
            
            if (branch._dataIndex == 0)
                return false;


            seek(branch._dataIndex);
            Index size;

            read(&size, sizeof(Index));
            
            return size > 0;
        }
        
        inline void setData(Index dataIndex, const std::string& source)
        {
            ScopedFileLock lock(*this);
            seek(dataIndex);
            Index size = source.size();
            write(&size, sizeof(Index));
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
        
        virtual Index rootIndex() {
            return 0;
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