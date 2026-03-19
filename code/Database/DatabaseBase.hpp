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
#include "Locks.hpp"

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
    
    private:

        Index _pageSize = 0;
        
        std::map<Index, Index> _lockCounts;

       // Cache* _cache = nullptr;
        Locks* _locks = nullptr;
    public:
        
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
           // _cache = new Cache("BranchCache", *this, _pageSize);
            _locks = new Locks("BranchLocks", *this, _pageSize * 10);
        }

        virtual ~Database()
        {
            assert (_lockCounts.size() == 0);
            delete _locks;

        }
        
    public:

        using LockFile::lock;
        using LockFile::unlock;
        
        void lock(Index lockIndex)
        {

            if (_lockCounts[lockIndex]++ == 0) {
LockFile::lock();
return;
                assert(_lockCounts[lockIndex] == 1);
                locks().lock(lockIndex);
            }
        }
        
        
        void unlock(Index lockIndex)
        {
            bool unlock = false;
            if (_lockCounts.find(lockIndex) !=
                _lockCounts.end())
            {
                    
                if (_lockCounts[lockIndex] > 0) {
                    if (--_lockCounts[lockIndex] == 0)
                        unlock = true;
                }
            }
                    
            if (unlock)
            {
 //               locks().unlock(lockIndex);
 LockFile::unlock();
 
                _lockCounts.erase(lockIndex);
            }
        
        }
        
        void unlock() {
            LockFile::unlock();
            _locks->clear();
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
 
        inline Index allocate(const BString& data)
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
            
            
            if (index != 0)
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
            
            if (branch._left)
                deleteBranch(branch._left);
                    
            if (branch._right)
                deleteBranch(branch._right);
            
            
            // Reclaim space here
            
        }
        
    public:
        
        void deleteData(Index index)
        {

            Branch branch = getBranch(index);

            if (branch._dataIndex) {
                // Reclaim space here
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
            
            seek(index);
            read(&branch, sizeof(Branch));
            
            return branch;


        }
        
        


        inline void setBranch(Index index, const Branch& branch)
        {
            seek(index);
            write(&branch, sizeof(Branch));
        }
    
        
        inline BString getData(Index dataIndex)
        {

            if (dataIndex == 0)
                return "";
                
            seek(dataIndex);
            Index size;

            read(&size, sizeof(Index));
            BString buffer(size, '\0');
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
        
        inline void setData(Index dataIndex, const BString& source)
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