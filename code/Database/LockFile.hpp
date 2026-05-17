#ifndef BEE_FISH__DATABASE__LOCK_FILE_HPP
#define BEE_FISH__DATABASE__LOCK_FILE_HPP


#include "File.hpp"
#include <mutex>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/chrono.hpp>

namespace BeeFishDatabase
{
    using namespace boost::interprocess;
    
    class LockFile : public File
    {
    protected:
        Size _lockCount = 0;
        BString _mutexName;
        named_mutex* _mutex;
        
        
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
                BString("LockFile");
            
            _mutex = 
                new named_mutex(
                    open_or_create,
                    _mutexName.c_str()
                );
            
        }
        
        virtual ~LockFile()
        {
            assert(_lockCount == 0);
            delete _mutex;
             
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
            time_duration diff = seconds(5);

            // 3. Add the duration to the current time
            ptime timeout = now + diff;
    
            file._mutex
                ->timed_lock(
                    timeout
                );
            
            file._mutex->unlock();
            
            return;
            /*
            std::filesystem::path path(filename);
            
            std::hash<std::string> hasher;

            // Since path character '/' isnt allowed
            // this will use the filename
            // hash instead
                
            std::size_t hashedValue =
                hasher(path.string());
            std::stringstream stream;
            stream << hashedValue;
            
            BString mutexName =
                BString(stream.str()) +
                BString("LockFile");
            
            boost::interprocess::named_mutex::remove(mutexName.c_str());
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