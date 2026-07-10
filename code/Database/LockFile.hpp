#ifndef BEE_FISH__DATABASE__LOCK_FILE_HPP
#define BEE_FISH__DATABASE__LOCK_FILE_HPP


#include "File.hpp"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <mutex>
#include <atomic>
#include <iostream>
#include <utility>
namespace BeeFishDatabase
{
    namespace bip = boost::interprocess;
    
    class LockFile : public File
    {
    protected:
        Size _lockCount = 0;
        BString _mutexName;
        bip::named_mutex* _mutex;
        
        BString _sharedMemoryName;
            bip::managed_shared_memory*
                _sharedMemory;
                
        // 3. Define Types
        // Key type can be a standard integer
        typedef Index KeyType; 
        
        // Value type must be the atomic type
        typedef std::atomic<Index> 
            MappedType; 
        
        // The value type of the map allocator must pair a const key with the value
        typedef std::pair<
            const KeyType,
            MappedType
        > ValueType;

        // 4. Define the Allocator type using Interprocess segment manager
        typedef bip::allocator<ValueType, bip::managed_shared_memory::segment_manager> ShmemAllocator;

        // 5. Define the Map type
        typedef bip::map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> SharedMap;
        
        SharedMap* _sharedMap;
        
        
    public:
        struct ScopedLock
        {
            LockFile& _lockFile;
            
            
            ScopedLock(LockFile& lockFile) :
                _lockFile(lockFile)
            {
                _lockFile.lock();
            }
            
            ~ScopedLock() {
                _lockFile.unlock();
            }
        };
        
    public:
    
        LockFile(
            const std::string& filename = "",
            bool readOnly = false
        ) :
            File(filename, readOnly)
        {
            std::filesystem::path path(_filename);
            
            std::hash<std::string> hasher;

            // Since path character '/' isnt allowed
            // this will use the filename
            // hash instead
                
            std::size_t hashedValue =
                hasher(path.string());
            std::stringstream stream;
            stream << hashedValue;
            
            _mutexName =
                BString(stream.str()) +
                BString("LockFileMutex");
            
            _mutex = 
                new bip::named_mutex(
                    bip::open_or_create,
                    _mutexName.c_str()
                );
                
            _sharedMemoryName =
                BString(stream.str()) +
                BString("LockFileData");
                
            // 1. Create or Open the Shared Memory segment
            _sharedMemory = new  bip::managed_shared_memory(
                bip::open_or_create, 
                _sharedMemoryName.c_str(),
                65536
            );
            
            //bip::shared_memory_object::remove(shm_name);

            // 6. Initialize the allocator and construct the map in shared memory
          //  ShmemAllocator alloc_inst(_sharedMemory->get_segment_manager());
           // _sharedMap = _sharedMemory->construct<SharedMap>("AtomicMap")(std::less<KeyType>(), alloc_inst);
/*
        // 7. Inserting elements into the map
        // Note: std::atomic is not copyable/movable, so you must use piecewise_construct or emplace
        my_map->emplace(std::piecewise_construct, 
                        std::forward_as_tuple(42), 
                        std::forward_as_tuple(100)); // Key: 42, Value: 100

        my_map->emplace(std::piecewise_construct, 
                        std::forward_as_tuple(84), 
                        std::forward_as_tuple(200)); // Key: 84, Value: 200

        // 8. Safely manipulate atomic integers across processes
        // Thread/Process 1
        (*my_map)[42].fetch_add(5, std::memory_order_relaxed); 

        // Thread/Process 2
        int current_val = (*my_map)[42].load(std::memory_order_relaxed);
        std::cout << "Value at key 42: " << current_val << std::endl; // Prints 105

    } catch (const std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        bip::shared_memory_object::remove(shm_name);
        return 1;
    }

    bip::shared_memory_object::remove(shm_name);
    return 0;
    */
                            

        }
        
        virtual ~LockFile()
        {
            delete _mutex;
            delete _sharedMemory;
             
        }

        void lock() {

            
            if (_lockCount++ == 0) {
               // flock(_fileNumber, LOCK_EX);
                _mutex->lock();
            }
        }
         
        void unlock() {
            
            if (_lockCount > 0)
                --_lockCount;
                
            if (_lockCount == 0)
            {
            //  flock(_fileNumber, LOCK_UN);
                _mutex->unlock();
            }
            
        }
        
        static void unlock(const std::string& filename)
        {
            LockFile file(filename);
            
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
            
            return;
            

        }
        
/*

namespace bip = boost::interprocess;

// 1. Enforce safety checks at compile time
static_assert(ATOMIC_INT_LOCK_FREE == 2, "std::atomic<int> must be completely lock-free for shared memory!");

int main() {
    const char* shm_name = "MySharedMemory";
    const std::size_t shm_size = 65536;

    // Ensure clean state for demonstration
    bip::shared_memory_object::remove(shm_name);

    try {
        // 2. Create the managed shared memory segment
        bip::managed_shared_memory segment(bip::create_only, shm_name, shm_size);

        // 6. Initialize the allocator and construct the map in shared memory
        ShmemAllocator alloc_inst(segment.get_segment_manager());
        MyShmMap* my_map = segment.construct<MyShmMap>("MyAtomicMap")(std::less<KeyType>(), alloc_inst);

        // 7. Inserting elements into the map
        // Note: std::atomic is not copyable/movable, so you must use piecewise_construct or emplace
        my_map->emplace(std::piecewise_construct, 
                        std::forward_as_tuple(42), 
                        std::forward_as_tuple(100)); // Key: 42, Value: 100

        my_map->emplace(std::piecewise_construct, 
                        std::forward_as_tuple(84), 
                        std::forward_as_tuple(200)); // Key: 84, Value: 200

        // 8. Safely manipulate atomic integers across processes
        // Thread/Process 1
        (*my_map)[42].fetch_add(5, std::memory_order_relaxed); 

        // Thread/Process 2
        int current_val = (*my_map)[42].load(std::memory_order_relaxed);
        std::cout << "Value at key 42: " << current_val << std::endl; // Prints 105

    } catch (const std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
        bip::shared_memory_object::remove(shm_name);
        return 1;
    }

    bip::shared_memory_object::remove(shm_name);
    return 0;
}

*/
        
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