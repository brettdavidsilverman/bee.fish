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
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility> // for std::pair

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>

#include "../Miscellaneous/Debug.hpp"
#include "../b-string/b-string.h"
#include "Version.hpp"
#include "File.hpp"
#include "LockFile.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "LockFile.hpp"
#include "LeastRecentlyUsedCache.hpp"

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
        

        class AutoUnlockMutex :
            
            public interprocess_mutex
            
        {
        protected:
            bool _locked = false;
            
        public:
            AutoUnlockMutex() :
                interprocess_mutex()
            {
            }

            virtual ~AutoUnlockMutex()
            {
                if (_locked)
                    interprocess_mutex::unlock();
            }
            
            void lock() {
                interprocess_mutex::lock();
                _locked = true;
            }
            
            void unlock() {
                interprocess_mutex::unlock();
                _locked = false;
            }
            
            

        };

        typedef interprocess_mutex Mutex;
    private:
        typedef LeastRecentlyUsedCache<Index, Mutex> Locks;
        typedef LeastRecentlyUsedCache<Index, Branch> Cache;
        
        
        Index _pageSize = 0;
        
        std::map<Index, Index> _lockCounts;

        Cache* _cache = nullptr;
        Locks* _locks = nullptr;
    public:
        
        Cache& cache() {
            return *_cache;
        }
        
        Locks& locks() {
            return *_locks;
        }
        
    public:
        Database(
            string filePath = "",
            bool readOnly = false,
            const Index pageSize = getPageSize()
        ) :
            LockFile(
                filePath,
                readOnly
            ),
            _pageSize(pageSize)
            
        {
            createSharedMemoryObjects();
        }

        virtual ~Database()
        {
            delete _cache;
            delete _locks;
            
            /*
            shared_memory_object::remove(_sharedMemoryName.c_str());
            
            
               
            */
            //unlock();
        }
        
    public:
        void createSharedMemoryObjects()
        {
            _cache = new Cache("BranchCache", *this, 100000);
            _locks = new Locks("BranchLocks", *this, 100);
        }
        
        using LockFile::lock;
        using LockFile::unlock;
        
        void lock(Index lockIndex)
        {
            if (_lockCounts[lockIndex]++ == 0) {
#ifdef VERBOSE
cout << lockIndex << " " << this_thread::get_id() << " +" << endl;
#endif
                assert(_lockCounts[lockIndex] == 1);
                locks()[lockIndex].lock();
            }
        }
        
        
        void unlock(Index lockIndex)
        {

            bool unlock = false;
            if (_lockCounts.find(lockIndex) !=
                _lockCounts.end())
            {
                    
                if (_lockCounts[lockIndex] > 0)
                    _lockCounts[lockIndex]--;
            
                if (_lockCounts[lockIndex] == 0)
                    unlock = true;
            }
                    
            if (unlock)
            {
#ifdef VERBOSE
cout << lockIndex << " " << this_thread::get_id() << " -" << endl;
#endif
                locks()[lockIndex].unlock();
                _lockCounts.erase(lockIndex);
            }
        
        }
        
        void clear() {
            _cache->clear();
            _locks->clear();
        }
        
        /*
    
        inline managed_shared_memory* createSharedMemory(Index capacity)
        {
            
            std::hash<std::string> hasher;
            std::filesystem::path path = filename();
            // Since path character '/' isnt allowed
            // this will use the filename
            // hash instead
            std::size_t hashedValue = hasher(
                path.string()
            );
            std::stringstream stream;
            stream << hashedValue;
            _sharedMemoryName = 
                BString(stream.str()) +
                BString("Database");
            
            Index memorySize = capacity * 4 * sizeof(MapType);
            
            shared_memory_object::remove(_sharedMemoryName.c_str());
            
            return new
                managed_shared_memory(open_or_create, _sharedMemoryName.c_str(), memorySize);
            
        }
        
        inline void destroySharedMemory()
        {
            
            delete _sharedMemory;
            shared_memory_object::remove(_sharedMemoryName.c_str());
            
        }
        
*/
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
        
    public:
        
        void deleteData(Index index)
        {
            Branch branch = getBranch(index);
            
            if (branch._dataIndex) {
                branch._dataIndex = 0;
                setBranch(index, branch);
            }
            
        }
        
        
        
        Branch getBranch(Index index)
        {

            
            
            Branch branch;
            
            if (size() == 0) 
            {
                ScopedFileLock lock(*this);
                
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
               // ScopedFileLock lock(*this);
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
           // ScopedFileLock lock(*this);
            cache().put(index, branch);
            seek(index);
            write(&branch, sizeof(Branch));
            
            
        }
    
        
        inline std::string getData(Index dataIndex)
        {

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
                 << "LockFile: "
                 << db._mutexName << endl
                 << "Branch size: "
                 << sizeof(Branch) << endl
                 << "Size: "
                 << db.size() << endl;
             
            return out;
        }
        
    };

    

}

#endif