#ifndef BEE_FISH_DATABASE__FILE_H
#define BEE_FISH_DATABASE__FILE_H

#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Size.hpp"

namespace BeeFishDatabase {

   using namespace std;
   
   class File {
   protected:
      int  _fileNumber = 0;
      bool _isNew = false;
      bool _isTemp = false;
      string _filename;

   public:
      File(
         const string& filename = ""
      ) : _filename(filename)
      {

         
         if (_filename == "") {
            std::string temp =
               TEMP_DIRECTORY
               "bee.fish.XXXXXX";
               
            _fileNumber = mkstemp(temp.data());
            _filename = temp;
            _isNew = true;
            _isTemp = true;
         }
         else
            open();

      }

      File(const File& source) :
         _filename(source._filename)
         
      {
         _isNew = false;
         _isTemp = false;
         open();
      }
      
         
      virtual ~File() {
         close();
         if (_isTemp && exists())
            ::remove(_filename.c_str());
      }
      
      bool isNew() const
      {
         return _isNew;
      }
      
      const string& filename() const
      {
         return _filename;
      }
      
      
      Size seek
      (
         long int offset,
         int origin = SEEK_SET
      ) const
      {

         ssize_t result =
            lseek(
               _fileNumber,
               offset,
               origin
            );
            
         if (result < 0)
         {
            string error =
               std::strerror(errno);

            stringstream stream;
            stream
               << "Error seeking file." << endl
               << "\tSize: " << size() << endl
               << "\tOffset u: " << offset << endl
               << "\tOffset s: " << (long int)offset << endl
               << "\tOffset -s: " << -offset << endl
               << "\tOrigin: " << origin << endl
               << "\tError: " << error << endl;

            throw runtime_error(
               stream.str()
            );
         }
         
         return result;
      }
      
      void close()
      {
         if (_fileNumber > 0) {
            ::close(_fileNumber);
            _fileNumber = 0;
         }
      }
      
      Size read
      (
         void * ptr,
         size_t size
      ) const
      {
         ssize_t result = ::read(_fileNumber, ptr, size);

         if (result < 0)
         {
            string error =
               std::strerror(errno);

            stringstream stream;

            stream 
               << "Error reading file: " 
               << "Requested " << size << ". "
               << "Returned " << result << ". "
               << error;

            throw runtime_error(
               stream.str()
            );
         }

         return result;
      }
      
      
      Size write
      (
         const void * ptr,
         size_t size
      )
      {
         size_t result =
            ::write(
               _fileNumber,
               ptr,
               size
            );

         if (result != size)
         {
            string error =
               std::strerror(errno);
            error =
               "Error writing file." +
               error;
            throw runtime_error(
               error
            );
         }

         return result;
      }
      
      Size size() const
      {
         return seek(0, SEEK_END);
      }
      
   protected:

      bool exists()
      {
         return (
            access(
               _filename.c_str(),
               F_OK
            ) == 0
         );
      }

      void open() {
         // Open the file
         _fileNumber = ::open(_filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

         if (_fileNumber <= 0) {

            string error =
               std::strerror(errno);
            error =
               "Error opening file " + _filename + ". " + 
               error;
            throw runtime_error(
               error
            );

         }

      }
   };

}

#endif
