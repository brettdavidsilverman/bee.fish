#ifndef BEE_FISH_DATABASE__FILE_H
#define BEE_FISH_DATABASE__FILE_H

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
      FILE* _file = NULL;
      bool _isNew;
      bool _isTemp;
      string _filename;

   public:
      File(
         const string& filename = "",
         const Size initialSize = 0
      ) : _file(NULL),
          _filename(filename)
      {
         if (_filename == "") {
            std::string temp = TEMP_DIRECTORY;
            temp += "bee.fish.XXXXXX";
            int fileNo = mkstemp(temp.data());
            _filename = temp;
            _isNew = true;
            _isTemp = true;
            ::close(fileNo);
            open();
            resize(initialSize);
         }
         else {

            // Create the file if it
            // doesnt exist
            if (exists() == false) {
               create(initialSize);
               _isNew = true;
            }
            else
               _isNew = false;
               
            _isTemp = false;

            open();

         }


      }

      File(const File& source) :
         _file(NULL),
         _filename(source._filename)
         
      {
         _isNew = false;
         _isTemp = false;
         open();
      }
      
         
      ~File() {
         close();
         if (_isTemp && exists())
            ::remove(_filename.c_str());
      }
      
      Size fileSize() const
      {
         return size();
      }

      bool isNew() const
      {
         return _isNew;
      }
      
      const string& filename() const
      {
         return _filename;
      }

      virtual Size seek
      (
         Size offset,
         int origin = SEEK_SET
      ) const
      {
         
         size_t result =
            fseek(
               _file,
               offset,
               origin
            );
            
         if (result != 0)
         {
            string error =
               std::strerror(errno);
            error =
               "Error seeking file." +
               error;
            throw runtime_error(
               error
            );
         }
         
         return offset;
      }
      
      virtual void close()
      {
         if (_file) {
            fclose(_file);
            _file = NULL;
         }
      }
      
      Size read
      (
         void * ptr,
         size_t count,
         size_t size
      ) const
      {
         size_t result =
            fread(
               ptr,
               count,
               size,
               _file
            );

         return result;
      }
      
      
      Size write
      (
         const void * ptr,
         size_t count,
         size_t size
      )
      {
         size_t result =
            fwrite(
               ptr,
               count,
               size,
               _file
            );

         if (result != (count * size))
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

      void create(const Size initialSize)
      {
         FILE* file = fopen(
            _filename.c_str(), "w+"
         );
         
         if (file == NULL) {
            throw runtime_error(
               "Couldn't create file " +
               _filename
            );
         }
   
         resize(
            fileno(file),
            initialSize
         );
   
         fclose(file);

      }

      void open() {
         // Open the file
         _file = fopen(
            _filename.c_str(), "rw+"
         );
      
         if (_file == NULL) {
            throw runtime_error(
               "Couldnt open file " +
               _filename
            );
         }

         _fileNumber = fileno(_file);
      }

      Size resize(const Size newSize)
      {

         resize(
            _fileNumber,
            newSize
         );
         
         return size();
   
      }

      Size size() const
      {
         Size size = getFileSize(_fileNumber);
         return size;
      }
      
      int _fileNumber = -1;
      
   private:
      static void resize(
         int fileNumber,
         Size newSize
      )
      {
         int result =
            ftruncate(fileNumber, newSize);

         if (result != 0)
         {
            string str = "Couldn't resize file. ";
            str += to_string(newSize) +
                   strerror(errno);
            
            throw runtime_error(str);
         }
         
      }
      
   private:
      
      static Size getFileSize(int file)
      {
         struct stat buffer;
         fstat(file, &buffer);
         return buffer.st_size;
      }
  public:
  
    
     
   };

}

#endif
