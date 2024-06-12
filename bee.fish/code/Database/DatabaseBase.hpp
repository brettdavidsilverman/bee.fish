#ifndef BEE_FISH_DATABASE__DATABASE_BASE_HPP
#define BEE_FISH_DATABASE__DATABASE_BASE_HPP

#include "Config.hpp"

#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <filesystem>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include "../Miscellaneous/Debug.hpp"
#include "Version.hpp"
#include "File.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "Data.hpp"

using namespace std;
using namespace std::literals;

namespace BeeFishDatabase {
   
   using namespace BeeFishMisc;
   using namespace boost::interprocess;
   
   typedef std::mutex Mutex;
   
   // Store [left, right] branch elements.
   // A zero is stored if the branch
   // hasnt been visited yet.
   // The _next points to the furthest element.
   class Database :
      public File
   {
   public:
      typedef PowerEncoding Encoding;
       
      struct Header
      {
         char _version[256];
         Size _pageSize;
      } _header;
    
      AtomicIndex* _nextIndex;
      Size         _pageSize;
      Mutex        _mutex;
      Mutex        _writeMutex;

      std::string          _nextIndexFilename;
      shared_memory_object _sharedMemory;
      mapped_region        _region;
      
   public:
      Database(
         string filePath = "",
         const Size pageSize = getPageSize()
      ) :
         File(
            filePath
         ),
         _pageSize(pageSize)
      {
#ifdef DEBUG
         Debug debug;
         debug << now() << " "
               << "Database(\""
               << escape(_filename)
               << "\")"
               << "\r\n";
#endif

         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();

         initializeNextIndex();
         
      }

      virtual ~Database()
      {
         shared_memory_object::remove(
            _nextIndexFilename.c_str()
         );
         
#ifdef DEBUG
         Debug debug;

         debug << now() << " "
               << "~Database(\""
               << escape(_filename)
               << "\")"
               << "\r\n";
#endif
      }
      
      virtual void initializeNextIndex()
      {
         boost::interprocess
            ::permissions permissions;
         permissions.set_default();
         
         _nextIndexFilename = _filename
            + ".next_index";
            
         std::replace(
            _nextIndexFilename.begin(),
            _nextIndexFilename.end(),
            '/', 
            '_'
         );
         
         _sharedMemory = 
            shared_memory_object(
               boost::interprocess
                  ::open_or_create,
               _nextIndexFilename.c_str(),
               boost::interprocess
                  ::mode_t::read_write,
               permissions
            );
            
         cerr << "Create name: " << _nextIndexFilename << endl;
         
         bool createdSharedMemory = false;
         offset_t sharedMemorySize = 0;
         if (
             !_sharedMemory.get_size(sharedMemorySize) ||
              sharedMemorySize == 0
            )
         {
             
             cerr << "Truncate" << endl;
         
            _sharedMemory.truncate(sizeof(AtomicIndex));
            createdSharedMemory = true;
         }
         
         
         // Map the whole shared memory in this process
         _region = mapped_region(
            _sharedMemory,
            boost::interprocess
               ::mode_t::read_write,
            0,
            sizeof(AtomicIndex)
         );
         
         if (createdSharedMemory)
         {
            // Initialize the shared memory
            // with current branch count
            Size branchCount =
               floor((double)(size() - sizeof(Header)) /
               (double)sizeof(Branch));
         
            AtomicIndex* atomicIndex =
               new AtomicIndex((Index)branchCount);
            memcpy(_region.get_address(), atomicIndex, sizeof(AtomicIndex));
         
            
         }
         
         _nextIndex =
            (AtomicIndex*)
            _region.get_address();

         

      }
      
      
   private:

      virtual void initializeHeader()
      {
         std::scoped_lock lock(_mutex);
         memset(&_header, 0, sizeof(Header));
         strcpy(_header._version, DATABASE_VERSION);
         _header._pageSize = _pageSize;
         seek(0);
         write(&_header, 1, sizeof(Header));
      }
      
      virtual void checkHeader()
      {
         std::scoped_lock lock(_mutex);
         seek(0);
         read(&_header, 1, sizeof(Header));
         
         if (strcmp(_header._version, DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Program version ";
            error += DATABASE_VERSION;
            error += ". File version ";
            error += _header._version;
            throw runtime_error(error);
         }

         if (_header._pageSize != _pageSize)
         {
            std::stringstream stream;
            stream
               << "Invalid page size."
               << "Program is " << _pageSize
               << ". Database is " << _header._pageSize
               << ".";
            throw runtime_error(stream.str());
         }
         


      }
      
   public:

      Size pageSize() const {
         return _pageSize;
      }

      inline Index getNextIndex()
      {
         return ++(*_nextIndex);

      }
 
      inline Index allocate(Size byteSize)
      {
         
         std::scoped_lock lock(_mutex);

         Size size = sizeof(Size) + byteSize;
  
         Index branchCount =
            ceil((double)size /
                 (double)sizeof(Branch));

         Index dataIndex;
         dataIndex = getNextIndex();
         
         *_nextIndex += branchCount;

         return dataIndex;
            
      }
      
      inline Branch getBranch(Index index)
      {
         std::scoped_lock lock(_mutex);
         Size offset = sizeof(Header) + index * sizeof(Branch);
         
         Branch branch;
         if (offset + sizeof(Branch) > size())
         {
            // Seek past end of file
            // This will write a cleared branch
            seek(offset + sizeof(Branch));
            //write(&branch, 1, sizeof(Branch));
         }
         
         seek(offset);
         
         read(&branch, 1, sizeof(Branch));

         return branch;
         
      }


      inline void setBranch(Index index, const Branch& branch)
      {
         std::scoped_lock lock(_mutex);
         seek(sizeof(Header) + index * sizeof(Branch));
         write(&branch, 1, sizeof(Branch));
      }

      
      inline Data getData(Index dataIndex)
      {
         if (dataIndex == 0)
            return Data();
            
         std::scoped_lock lock(_mutex);
         seek(sizeof(Header) + dataIndex * sizeof(Branch));
         Size size;
         read(&size, 1, sizeof(Size));
         Data data(size);
         read(data.data(), 1, size);

         return data;
      }

      
      inline void setData(Index dataIndex, const Data& source)
      {
         std::scoped_lock lock(_mutex);

         seek(sizeof(Header) + dataIndex * sizeof(Branch));
         Size size = source.size();
         write(&size, 1, sizeof(Size));
         write(source.data(), 1, size);
         
      }
 
      
      string version() const {
         return _header._version;
      }

   protected:
   
      
      friend ostream& operator <<
      (ostream& out, const Database& db)
      {

         out << "Database: " 
             << db._header._version
             << endl
             << "Filename: "
             << db._filename
             << endl
             << "Next: "
             << (Index)(*(db._nextIndex))
             << endl
             << "Branch size: "
             << sizeof(Branch)
             << endl
             << "Size: "
             << db.size()
             << endl;
          
         return out;
      }

   };

}

#endif