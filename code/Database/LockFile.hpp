#ifndef BEE_FISH__DATABASE__LOCK_FILE_HPP
#define BEE_FISH__DATABASE__LOCK_FILE_HPP


#include <utility>
#include <thread>
#include <boost/interprocess/sync/lock_options.hpp>

#include "../Miscellaneous/SharedMemory.hpp"
#include "../Id/Id.hpp"

#include "File.hpp"

namespace BeeFishDatabase
{
namespace bip = boost::interprocess;
using namespace BeeFishSharedMemory;

class LockFile :
    public File,
    public SharedMemory

{
public:

    
    
protected:
    // Define Types

    typedef Index KeyType;
    
    typedef bip::interprocess_recursive_mutex
        Mutex;

    // Mutex with atomic counter
    struct MapValueType {
        Mutex _mutex;
            /*
        bip
        ::scoped_lock<
            bip
            ::interprocess_recursive_mutex
        > _lock;
        */
        std::atomic<Index> _counter = 0;
        MapValueType()/* :
            _lock(
                _mutex,
                bip::defer_lock
            )*/
        {
        }
    };

    // The value type of the map allocator must pair a const key with the value
    typedef std::pair<
    const KeyType,
          MapValueType
          > ValueType;

    // Define the Allocator type
    typedef bip::allocator<ValueType, bip::managed_shared_memory::segment_manager> ShmemAllocator;

    // Define the Map type
    typedef bip::map<KeyType, MapValueType, std::less<KeyType>, ShmemAllocator> SharedMap;

    ShmemAllocator* _allocator;

    SharedMap* _sharedMap;
    
public:

    LockFile(
        const std::string& filename = "",
        bool readOnly = false
    ) :
        File(filename, readOnly),
        SharedMemory(
            std::filesystem::path(
                File::filename()
            )
        )

    {
        _allocator = new
        ShmemAllocator(_sharedMemory.get_segment_manager());

        _sharedMap =
            _sharedMemory
            .find_or_construct<SharedMap>(
                "LockFile.Map"
            )
            (
                std::less<KeyType>(),
                *_allocator
            );
    }

    virtual ~LockFile()
    {
        delete _allocator;
    }

    void lock(Index index) {
//cerr << index << "\tLOCK" << endl;

        ScopedLock lock(*this);
        
        Index counter = 
            (*_sharedMap)[index]._counter++;
            
      //  if (counter != 0)
      //      lock.unlock();
        
        
        
      //  lock.unlock();
        
        if (counter == 0) {
            (*_sharedMap)[index]._mutex.lock();
        }
        else {
            /*
            lock.swap(
                (*_sharedMap)[index]._lock
            );
            
            lock.try_lock();
            */
            lock.unlock();
            
            
            (*_sharedMap)[index]._mutex.lock();
            
        }


        
    }

    void unlock(Index index) {
//cerr << index << "\tUNLOCK" << endl;

        ScopedLock lock(*this);
        (*_sharedMap)[index]._mutex.unlock();
        
        if (--(*_sharedMap)[index]._counter == 0)
        {
            _sharedMap->erase(index);
        }

    }

    static void reset(const std::filesystem::path& path)
    {
        
       // BeeFishId::Timestamp::Memory::reset();

        LockFile file(path);
        /*
        boost::posix_time::ptime 
            timeout = 
                boost::posix_time::microsec_clock::universal_time() + 
                boost::posix_time::seconds(30);
            */
    cerr << "TRY_LOCK" << endl;
        file._mutex->try_lock();
    cerr << "UNLOCK" << endl;
        file._mutex->unlock();
    
    cerr << "SCOPED LOCK" << endl;
        ScopedLock lock(file);
            /*
        for (auto& pair : *file._sharedMap)
        {
            pair.second._lock.unlock();
        }
        */
    cerr << "SHAREDMAP CLEAR" << endl;
        file._sharedMap->clear();
    cerr << "SCOPED UNLOCK" << endl;
            
    }



    /*
            virtual void lock(Index position, Size length)
            {
                struct flock data;
                memset(&data, '\0', sizeof(data));
                data.l_type = F_WRLCK;
                data.l_whence = SEEK_SET;
                data.l_start = position;
                data.l_len = length;

                fcntl(_fileNumber, F_SETLKW, &data);

            }

            virtual void unlock(Index position, Size length)
            {
                struct flock data;
                memset(&data, '\0', sizeof(data));
                data.l_type = F_UNLCK;
                data.l_whence = SEEK_SET;
                data.l_start = position;
                data.l_len = length;

                fcntl(_fileNumber, F_SETLK, &data);

            }

    */
};

}


#endif