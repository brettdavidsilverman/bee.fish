#ifndef BEE_FISH_DATABASE__DATABASE_BASE_HPP
#define BEE_FISH_DATABASE__DATABASE_BASE_HPP

#include "Config.hpp"

#include <sys/mman.h>

#include <string.h>
#include <map>
#include <mutex>

#include "Version.hpp"
#include "File.hpp"
#include "Index.hpp"
#include "Branch.hpp"
#include "Data.hpp"

using namespace std;

namespace BeeFishDatabase {
   
   using namespace BeeFishMisc;

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
         Index  _nextIndex;
      };
    
      struct Tree
      {
         Header _header;
         Branch _root[];
      };
      
      Size    _incrementSize;
      Size    _size;
      Tree*   _tree;
      Branch* _root;
      Index*  _nextIndex;
      Size    _branchCount;
      Size    _pageSize;

   public:
      std::mutex _lock;
   
      Database(
         string filePath,
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
         
         mapFile();
         
         if (isNew())
         {
            initializeHeader();
         }
    
         checkHeader();
         
         cout << "Database "
              << filePath
              << endl;
      }
      
      Database(const Database& source) :
         File(source),
         _incrementSize(source._incrementSize),
         _size(source._size),
         _pageSize(source._pageSize)
      {
         mapFile();
      }
      
      ~Database()
      {
         munmap(_tree, _size);
      }
      
      
   private:

      virtual void initializeHeader()
      {
         Header& header = _tree->_header;
         //memset(&header, '\0', sizeof(Header));
         strcpy(header._version, DATABASE_VERSION);
         header._pageSize = _pageSize;
         header._nextIndex = Branch::Root;
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

         if (_tree->_header._pageSize != getPageSize())
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
         _size = fileSize();
         
         _tree = (Tree*)
            mmap(
               NULL,
               _size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _fileNumber,
               0
            );
            
         _root = _tree->_root;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
            
         _nextIndex = 
           &(_tree->_header._nextIndex);
      }
         
      
   public:
      Size pageSize() const {
         return _pageSize;
      }

      inline Index getNextIndex(const Index& parent = 0)
      {
         Index nextIndex =
            ++(*_nextIndex);
         
         if (parent > 0) {

            Branch& nextBranch =
               getBranch(nextIndex);

            nextBranch._parent = parent;
         }

         return nextIndex;
      }
  
      inline Index allocate(Size byteSize)
      {
         Size size = sizeof(Size) + byteSize;
  
         Index branchCount =
            ceil(size / sizeof(Branch));
            
         Index dataIndex = getNextIndex();
         
         (*_nextIndex) += (branchCount);
         
         // Check for resize
         if ( *_nextIndex >= _branchCount )
         {
            resize(_size + byteSize);
         }
        
         Data* data = getData(dataIndex);
         
         data->setSize(byteSize);
         
         return dataIndex;
            
      }
      
      inline Branch& getBranch(const Index& index)
      {
         if ( index >= _branchCount )
         {
            resize();
         }
         
         return _root[index];
      }
      
      inline Data* getData(const Index& dataIndex)
      {
         if (dataIndex == 0)
            return nullptr;
         
         Data* data =
            (Data*)
               (&(_root[dataIndex]));
            
         return data;
      }
 
      virtual Size resize(
         Size size = 0
      )
      {
            
         
         if (size == 0)
            size = File::size() +
                   _incrementSize;
         
         if (size <= _size)
            size = _size;
         else
            size = File::resize(size);
         
         _tree = (Tree*)
            mremap(
               _tree,
               _size,
               size,
               MREMAP_MAYMOVE
            );
            
         _root = _tree->_root;
         
         _size = size;
         
         _branchCount =
            (_size - sizeof(Header)) /
            sizeof(Branch);
            
         _nextIndex = 
           &(_tree->_header._nextIndex);
           
         return _size;

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


