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

    //bip::interprocess_mutex* _mutex;
    bip::named_mutex _mutex;

    Index* _counter = nullptr;
    
    typedef bip::scoped_lock<
        bip::named_mutex
    >
    ScopedLockBase;
    
    class ScopedLock : ScopedLockBase
    {
    public:
        ScopedLock(SharedMemory& memory) :
            ScopedLockBase(memory._mutex)
        {
        }
    };
        

protected:

    SharedMemory(const BString& identifier, Index size = 65536) :
        _sharedMemoryName(identifier),
        _mutex(bip::open_or_create, "bee.fish.SharedMemory.mutex")
        
    {
        ScopedLock lock(*this);
        
        _sharedMemory = bip::managed_shared_memory(
            bip::open_or_create,
            _sharedMemoryName.c_str(),
            size
        );
        
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
            
//cerr << "~SHAREDMEMORY " << _counter->load() << endl;
        if (counter == 0)
        {
//cerr << "~SHAREDMEMORY " << _sharedMemoryName << endl;

            //_sharedMemory.deallocate(_counter);
            
            bip::shared_memory_object
            ::remove(
                _sharedMemoryName.c_str()
            );
            return;
        }
        
    }
    
    static void reset(const std::filesystem::path& path)
    {
        BString sharedMemoryName =
            SharedMemory::makeIdentifier(
                path
            );
            
cerr << "REMOVING " << sharedMemoryName << endl;
        
        bip::shared_memory_object
        ::remove(
            sharedMemoryName.c_str()
        );
    }
    
    static BString makeIdentifier(const std::filesystem::path& path)
    {
        

        std::hash<std::string> hasher;

        // Since path character '/' isnt allowed
        // this will use the filename
        // hash instead

        std::size_t hashedValue =
            hasher(path.string());
        std::stringstream stream;
        stream.imbue(std::locale::classic());

        stream << hashedValue;

        return
            BString("bee.fish.") +
            BString(stream.str());
    }

};


}
#endif


