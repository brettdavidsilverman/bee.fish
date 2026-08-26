#ifndef BEE_FISH_MISCELLANEOUS__SHARED_MEMORY_HPP
#define BEE_FISH_MISCELLANEOUS__SHARED_MEMORY_HPP

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>

#include "../b-string/b-string.h"

namespace BeeFishSharedMemory
{
namespace bip = boost::interprocess;


class SharedMemory
{
public:
    BString _sharedMemoryName;
    bip::managed_shared_memory
        _sharedMemory;

    
    Index* _counter = nullptr;
    
    typedef bip::interprocess_recursive_mutex
        Mutex;
        
    typedef bip::scoped_lock<Mutex>
        ScopedLockBase;
        
    
    Mutex* _mutex;
    
    class ScopedLock : public ScopedLockBase
    {
    public:
        ScopedLock(SharedMemory& memory) :
            ScopedLockBase(*memory._mutex)
        {
            
        }
    };

    

protected:

    SharedMemory(const BString& identifier, Index size = 65536) :
        _sharedMemoryName(identifier)
    {
        

        _sharedMemory = bip::managed_shared_memory(
            bip::open_or_create,
            _sharedMemoryName.c_str(),
            size
        );
        
        _mutex =
            _sharedMemory.find_or_construct
            <Mutex>
            ("SharedMemory.Mutex")();
  
        
        _counter =
            _sharedMemory
            .find_or_construct<
                Index
            >
            ("SharedMemoryCounter")(0L);

        ++(*_counter);

    }
    
public:
    SharedMemory(const char* identifier) :
        SharedMemory(BString(identifier))
    {
    }

    SharedMemory(const std::filesystem::path& path) :
        SharedMemory(
            makeIdentifier(path)
        )
    {
    }

    ~SharedMemory()
    {

        ScopedLock lock(*this);
        
        Index counter =
            --(*_counter);
            
        if (counter == 0)
        {
            bip::shared_memory_object
            ::remove(
                _sharedMemoryName.c_str()
            );
            return;
        }
        
    }
    
    static void reset(const BString& identifier)
    {
        
        SharedMemory memory(
            identifier
        );
        
        boost::posix_time::ptime 
            timeout = 
                boost::posix_time::microsec_clock::universal_time() + 
                boost::posix_time::seconds(30);
            
        memory._mutex
            ->timed_lock(
                timeout
            );
            
        memory._mutex->unlock();
            
    }
    /*
    static void reset(const std::filesystem::path& path)
    {
        reset(makeIdentifier(path));
    }
    */
    static BString makeIdentifier(const std::filesystem::path& path)
    {
        
/*
        std::hash<std::string> hasher;

        // Since path character '/' isnt allowed
        // this will use the filename
        // hash instead

        std::size_t hashedValue =
            hasher(path.string());
        std::stringstream stream;
        stream.imbue(std::locale::classic());

        stream << hashedValue;
*/

        return
            BString("bee.fish") +
            BString(path.string())
                .replace("/", ".");
    }

};


}
#endif


