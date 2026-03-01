#ifndef BEE_FISH__DATABASE__LOCK_FILE_HPP
#define BEE_FISH__DATABASE__LOCK_FILE_HPP


#include "File.hpp"
#include <mutex>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace BeeFishDatabase
{
    using namespace boost::interprocess;
    
    class LockFile : public File
    {
    protected:
        Size _lockCount = 0;
        BString _mutexName;
        managed_shared_memory* _sharedMemory = nullptr;
        interprocess_mutex* _mutex = nullptr;
        
    public:
        struct ScopedFileLock
        {
            LockFile& _lockFile;

            ScopedFileLock(LockFile& lockFile) :
                _lockFile(lockFile)
            {
                _lockFile.lock();
            }
            
            ~ScopedFileLock() {
                _lockFile.unlock();
            }
        };
        
    public:
    
        LockFile(const std::string& filename = "") :
            File(filename)
        {
            try {
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
            

                //shared_memory_object::remove(_mutexName.c_str());
                Index memorySize = 1024;
                _sharedMemory = new
                    managed_shared_memory(
                        open_or_create, 
                        _mutexName.c_str(), 
                        memorySize
                    );
                    
                _mutex = 
                    _sharedMemory->find_or_construct
                    <interprocess_mutex>("mutex")();
                
            }
            catch (std::exception& ex)
            {
                cerr << "LockFile fail " << ex.what() << endl;
                throw ex;
            }
        }
        
        virtual ~LockFile()
        {
            assert(_lockCount == 0);
            delete _sharedMemory;
             
        }

        virtual void lock() {
            
            if (_lockCount++ == 0) {
               // flock(_fileNumber, LOCK_EX);
                _mutex->lock();
            }
        }
         
        virtual void unlock() {
            
            if (_lockCount > 0)
                --_lockCount;
                
            if (_lockCount == 0)
            {
            //  flock(_fileNumber, LOCK_UN);
                _mutex->unlock();
            }
            
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