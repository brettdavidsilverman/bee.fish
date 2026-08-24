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

    // Mutex with atomic counter
    struct MapValueType {
        bip::interprocess_recursive_mutex
            _mutex;
        std::atomic<Index> _counter = 0;
        MapValueType()
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

        ScopedLock lock(*this);
        
       // Index counter = 
            (*_sharedMap)[index]._counter++;
            
      //  if (counter != 0)
            lock.unlock();
        
        (*_sharedMap)[index]._mutex.lock();
    
        /*
        else
        {
            lock.swap(
                (*_sharedMap)[index]._lock
            );
            (*_sharedMap)[index]._lock.lock();
        }
        */
        
    }

    void unlock(Index index) {

// cerr << index << " UNLOCK" << endl;

        ScopedLock lock(*this);
        
        (*_sharedMap)[index]._mutex.unlock();
        
        if (--(*_sharedMap)[index]._counter == 0)
        {

            _sharedMap->erase(index);
        }
      //  else
      //      (*_sharedMap)[index]._mutex.unlock();
// cerr << index << " UNLOCKED" << endl;


    }

    static void reset(const std::filesystem::path& path)
    {
        
       // BeeFishId::Timestamp::Memory::reset();

        SharedMemory::reset(makeIdentifier(path));

        /*
        using namespace boost::posix_time;

        // 1. Get the current local time
        ptime now = second_clock::universal_time();

        // 2. Define the duration to add (e.g., 5 seconds)
        time_duration diff = seconds(30);

        // 3. Add the duration to the current time
        ptime timeout = now + diff;

        file._mutex
            ->timed_lock(
                timeout
            );

        file._mutex->unlock();

        file._mutex->lock();
        for (auto it = file._sharedMap->begin();
             it != file._sharedMap->end();
             ++it)
        {
            try {
                it->second._mutex.unlock();
            }
            catch (...)
            {
            }
        }
        file._sharedMap->clear();

        file._mutex->unlock();

        return;
        */

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