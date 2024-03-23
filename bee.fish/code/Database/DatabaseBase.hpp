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
#include <boost/interprocess/sync/interprocess_mutex.hpp>
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

   typedef boost::interprocess::interprocess_mutex Mutex;
   
   // Store [left, right] branch elements.
   // A zero is stored if the branch
   // hasnt been visited yet.
   // The _next points to the furthest element.
   class Database :
      public File
   {
   public:
      typedef PowerEncoding Encoding;
   private:
      
      struct Header
      {
         char   _version[256];
         size_t _pageSize;
         AtomicIndex _nextIndex;
         Mutex _growLock;
         std::atomic_flag _writtingFlag;
      };
    
      struct Tree
      {
         Header _header;
         Branch _root[];
      };
      
      Size    _incrementSize;
      Size    _size;
      Tree*   _tree {nullptr};
      Branch* _root;
      AtomicIndex*  _nextIndex;
      Size    _branchCount;
      Size    _pageSize;
      Mutex*  _mutex;
   public:
      std::atomic_flag* _writtingFlag;
   public:
      Database(
         string filePath = "",
         const Size initialSize = 1000 * 1000,
         const Size incrementSize = 1000 * 1000,
         const Size pageSize = getPageSize()
      ) :
         File(
            filePath,
            initialSize
         ),
         _incrementSize(incrementSize),
         _pageSize(pageSize)
      {
         Debug debug;
         debug << now() << " "
               << "Database(\""
               << escape(_filename)
               << "\")"
               << endl;
               
         mapFile();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
         
      }
      
      Database(const Database& source) :
         File(source),
         _incrementSize(source._incrementSize),
         _size(source._size),
         _pageSize(source._pageSize)
      {
         mapFile();
      }
      
      virtual ~Database()
      {
         if (_tree)
            munmap(_tree, _size);

         Debug debug;
         debug << now() << " "
               << "~Database(\""
               << escape(_filename)
               << "\")"
               << endl;

      }
      
      
   private:

      virtual void initializeHeader()
      {
         Header& header = _tree->_header;
         //memset(&header, '\0', sizeof(Header));
         strcpy(header._version, DATABASE_VERSION);
         header._pageSize = _pageSize;
         memcpy(&(header._nextIndex), new AtomicIndex{Branch::Root}, sizeof(AtomicIndex));
         memcpy(&(header._growLock), new Mutex(), sizeof(Mutex));
         memcpy(&(header._writtingFlag), new std::atomic_flag{false}, sizeof(std::atomic_flag));
      }
      
      virtual void checkHeader()
      {
         if (strcmp(_tree->_header._version, DATABASE_VERSION) != 0)
         {
            std::string error = "Invalid file version.";
            error += " Program version ";
            error += DATABASE_VERSION;
            error += ". File version ";
            error += _tree->_header._version;
            throw runtime_error(error);
         }

         if (_tree->_header._pageSize != _pageSize)
         {
            std::stringstream stream;
            stream
               << "Invalid page size."
               << "Program is " << _pageSize
               << ". Database is " << _tree->_header._pageSize
               << ".";
            throw runtime_error(stream.str());
         }

      }
      
      virtual void mapFile()
      {
         if (_tree)
            munmap(_tree, _size);

         _size = size();
         
         _tree = (Tree*)
            mmap(
               NULL,
               _size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fileNumber,
               0
            );

         setMembers();
            
      }

      void setMembers() {

         _size = size();

         _root = _tree->_root;
         
         _branchCount =
            floor((float)(_size - sizeof(Header)) /
               (float)sizeof(Branch));
            
         _nextIndex = 
           &(_tree->_header._nextIndex);
           
         _mutex = &(_tree->_header._growLock);
         
         _writtingFlag = &(_tree->_header._writtingFlag);
         
      }
         
      
   public:

      Size pageSize() const {
         return _pageSize;
      }

      inline Index getNextIndex()
      {
         return ++(*_nextIndex);
         //return _nextIndex->load();

      }
 
      inline Index allocate(Size byteSize)
      {
         

         Size size = sizeof(Size) + byteSize;
  
         Index branchCount =
            ceil((float)size /
                 (float)sizeof(Branch));

         Index dataIndex;
         dataIndex = getNextIndex();
         
         (*_nextIndex) += (branchCount);

         // Check for resize
         while ( _branchCount < *_nextIndex  )
         {
            growFile();
         }
        
         Data* data = getData(dataIndex);
         
         data->_size = byteSize;
         
         return dataIndex;
            
      }
      
      inline Branch& getBranch(Index index)
      {
         if (index >= _branchCount)
         {
            growFile();
         }

         return _root[index];
         
      }

      inline const Branch& getBranch(Index index) const
      {
         if (index >= _branchCount)
            throw runtime_error("Index out of bounds");

         return _root[index];
         
      }
      
      inline Data* getData(Index dataIndex)
      {
         if (dataIndex == 0)
            return nullptr;
         
         Data* data =
            (Data*)
               (&(_root[dataIndex]));
            
         return data;
      }

      inline const Data* getData(Index dataIndex) const
      {
         if (dataIndex == 0)
            return nullptr;
         
         const Data* data =
            (Data*)
               (&(_root[dataIndex]));
            
         return data;
      }
 
      virtual Size growFile()
      {

         _mutex->lock();
         
         Size oldSize = _size;
         Size newSize = oldSize
            + _incrementSize;

         Debug debug;
         debug << now()
            << " Grow File \""
               << escape(_filename)
            << "\" "
            << (newSize / 1000 / 1000)
               << "Mb" << endl;
         
        
         if (newSize > size())
            newSize = File::resize(newSize);
         
         _tree = (Tree*)
            mremap(
               _tree,
               oldSize,
               newSize,
               MREMAP_MAYMOVE
            );
            
         assert(_tree);
            
         setMembers();

         _mutex->unlock();
         
         return size();

      }
      
      string version() const {
         return _tree->_header._version;
      }

   protected:
   
      
      friend ostream& operator <<
      (ostream& out, const Database& db)
      {

         out << "Database: " 
             << db._tree->_header._version
             << endl
             << "Filename: "
             << db._filename
             << endl
             << "Next: "
             << (unsigned long long)(db._tree->_header._nextIndex)
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


