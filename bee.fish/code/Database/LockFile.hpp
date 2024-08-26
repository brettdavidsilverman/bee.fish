#ifndef BEE_FISH__DATABASE__LOCK_FILE_HPP
#define BEE_FISH__DATABASE__LOCK_FILE_HPP


#include "File.hpp"

namespace BeeFishDatabase
{
   class LockFile : public File
   {
   protected:
      Size _lockCount = 0;
      std::mutex _mutex;
   public:
   
      LockFile(const std::string& filename) :
         File(filename)
      {
      }
      
      virtual ~LockFile()
      {
         assert(_lockCount == 0);
      }

      virtual void lock() {
         
         scoped_lock lock(_mutex);

         if (_lockCount++ == 0)
            flock(_fileNumber, LOCK_EX);
      }
       
      virtual void unlock() {
         
         scoped_lock lock(_mutex);

         if (--_lockCount == 0)
            flock(_fileNumber, LOCK_UN);
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

//         cerr << "Lock  " << position << endl;
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
         
//         cerr << "~Lock " << position << endl;
         fcntl(_fileNumber, F_SETLK, &data);

      }

*/
   };
   
}


#endif