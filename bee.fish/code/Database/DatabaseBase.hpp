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

   typedef recursive_mutex Mutex;
   
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
    
      AtomicIndex  _nextIndex;
      Size    _pageSize;
      Mutex _mutex;
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
         Debug debug;
         debug << now() << " "
               << "Database(\""
               << escape(_filename)
               << "\")"
               << "\r\n";
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();

         Size branchCount =
            floor((double)(size() - sizeof(Header)) /
               (double)sizeof(Branch));
            
         _nextIndex = branchCount;
         
      }

      virtual ~Database()
      {

         Debug debug;
         debug << now() << " "
               << "~Database(\""
               << escape(_filename)
               << "\")"
               << "\r\n";

      }
      
      
   private:

      virtual void initializeHeader()
      {
         lock_guard lock(_mutex);
          
         memset(&_header, 0, sizeof(Header));
         strcpy(_header._version, DATABASE_VERSION);
         _header._pageSize = _pageSize;
         seek(0);
         write(&_header, 1, sizeof(Header));
      }
      
      virtual void checkHeader()
      {
         lock_guard lock(_mutex);
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
         lock_guard lock(_mutex);
         
         return ++_nextIndex;

      }
 
      inline Index allocate(Size byteSize)
      {
         
         lock_guard lock(_mutex);

         Size size = sizeof(Size) + byteSize;
  
         Index branchCount =
            ceil((double)size /
                 (double)sizeof(Branch));

         Index dataIndex;

         dataIndex = getNextIndex();
         
         _nextIndex += branchCount;

         return dataIndex;
            
      }
      
      inline Branch getBranch(Index index)
      {

         lock_guard lock(_mutex);

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
         lock_guard lock(_mutex);

         seek(sizeof(Header) + index * sizeof(Branch));
         write(&branch, 1, sizeof(branch));

      }

      
      inline Data getData(Index dataIndex)
      {
         lock_guard lock(_mutex);

         if (dataIndex == 0)
            return Data();
         
         seek(sizeof(Header) + dataIndex * sizeof(Branch));
         Size size;
         read(&size, 1, sizeof(Size));
         Data data(size);
         read(data.data(), 1, size);

         return data;
      }

      
      inline void setData(Index dataIndex, const Data& source)
      {
         lock_guard lock(_mutex);

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
             << (Size)(db._nextIndex)
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