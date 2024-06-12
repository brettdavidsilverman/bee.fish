#ifndef BEE_FISH__DATABASE__PATH_HPP
#define BEE_FISH__DATABASE__PATH_HPP

#include <iostream>
#include <atomic>
#include <fcntl.h>

#include "../power-encoding/power-encoding.h"
#include "../Miscellaneous/Optional.hpp"
#include "../Script/Variable.hpp"
#include "Data.hpp"
#include "File.hpp"
#include "Branch.hpp"
#include "DatabaseBase.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;

namespace BeeFishDatabase {

   class Path :
      public PowerEncoding
   {
   public:
      Database* _database = nullptr;
      Index     _index = 0;
      int     _lockFile = 0;
      Index     _lockIndex = 0;
   public:

      Path( Database* database = nullptr,
            Index index = Branch::Root ) :
         PowerEncoding(),
         _database(database),
         _index(index),
         _lockFile(0),
         _lockIndex(0)
      {
      }

      Path( Database& database,
            Index index = Branch::Root ) :
         PowerEncoding(),
         _database(&database),
         _index(index),
         _lockFile(0),
         _lockIndex(0)
      {
      }

      Path(const Path& source) :
         PowerEncoding(),
         _database(source._database),
         _index(source._index),
         _lockFile(0),
         _lockIndex(0)
      {
      }
      
      virtual ~Path()
      {
         std::scoped_lock lock(_database->_writeMutex);
         if (_lockFile > 0)
         {
            cerr << "Unlocking index " << _lockIndex << ":" << "\r\n";
            close(_lockFile);
            remove(lockFilename(_lockIndex).c_str());
            _lockFile = 0;
            _lockIndex = 0;
         }
         
      }
      
      virtual bool readBit()
      {
         PowerEncoding::readBit();
            
         bool bit;
         
         Branch branch = getBranch();
         
         if (branch._left)
         {
            _index = branch._left;
            bit = 0;
         }
         else if (branch._right)
         {
            _index = branch._right;
            bit = 1;
         }
         else
         {
            throw std::runtime_error("Read past end of file");
         }
         
         return bit;
         
      }
      
      virtual bool peekBit()
      {
         const Branch branch = getBranch();
         
         if (branch._left)
            return 0;
         else if (branch._right)
            return 1;
               
         // Path peek bit past end of file
         throw std::runtime_error("Peek past end of file");

      }
      

      virtual void writeBit(bool bit)
      {

        // std::scoped_lock lock(_database->_writeMutex);
         Branch branch = getBranch();
         
         if (bit == 0)
         {
            if (branch._left == 0) {
               branch._left =
                  goLeft();
            }
            _index = branch._left;
            
         }
         else
         {
            if (branch._right == 0) {
               branch._right =
                  goRight();
            }
            _index = branch._right;
            
         }
        
         PowerEncoding::writeBit(bit);
         
         
      }
      
      Index goLeft()
      {
         lockBranch();
         Branch branch = getBranch();
         if (branch._left == 0) {
            branch._left =
               _database->getNextIndex();
            setBranch(branch);
         }
         return branch._left;
      }
      
      Index goRight()
      {
         lockBranch();
         Branch branch = getBranch();
         if (branch._right == 0) {
            branch._right =
               _database->getNextIndex();
            setBranch(branch);
         }
         return branch._right;
      }
      
      void lockBranch() {
          
         std::scoped_lock lock(_database->_writeMutex);
         
         if (_lockFile > -1)
            return;
            
         cerr << "Locking index " <<_index << "\r\n";
         _lockIndex = _index;
         _lockFile = open(lockFilename(_lockIndex).c_str(), O_RDWR | O_EXCL | O_CREAT);
        // cerr << "Lock file fd:" << _lockFile << "\r\n";
        // flock( fileno(_lockFile), LOCK_EX);
         
      }
      
      std::string lockFilename(Index lockIndex)
      {
         std::stringstream stream;
         stream <<
            _database->filename() <<
            "." <<
            lockIndex <<
            ".lock";
            
         return stream.str();
            
      }
      

      template<typename T>
      Path operator [] (const T& key)
      {
         Path path(*this);
         
         path << key;
         
         return path;
      }
 
      Path operator [] (const char* key)
      {
         return Path::operator[] 
            (std::string(key));
      }

      Size getDataSize() const
      {
            
         Branch branch = getBranch();
         
         if (branch._dataIndex)
         {
            const Data data =
               _database->getData(
                  branch._dataIndex
               );
               
            return data.size();
         }
         
         return 0;
      }
      
      bool hasData() const
      {
         Branch branch = getBranch();
         
         if (branch._dataIndex)
         {
            return true;
         }

         return false;
      }
      
      Data getData() {
          
         Branch branch = getBranch();
         
         if (branch._dataIndex)
         {
            Data source =
               _database->getData(
                  branch._dataIndex
               );

            return source;

         }

         assert(false);

      }
      
      const Data getData() const {

         Branch branch = getBranch();
         
         if (branch._dataIndex)
         {
            Data source =
               _database->getData(
                  branch._dataIndex
               );

            return source;

         }

         assert(false);

      }
      
      
      template<typename T>
      void getData(T& destination)
      {
         Data data = getData();
         destination = *(T*)data.data();
      }

      void getData(std::string& destination)
      {
         if (hasData()) {
            const Data data = getData();
            if (data.size())
               destination = std::string(
                  data.data(),
                  data.size()
               );
            else
               destination = "";
         }
         else
            destination = "";
      }

/*
      operator string() const
      {
         const Data data = getData();

         if (data.size() == 0)
            return "";

         return std::string(
            data.data(),
            data.size()
         );

      }
     */
      void setData(const void* value, Size size) {
         const std::string string((const char*)value, size);
         setData(string);
      }
         
      void setData(const std::string& value) {
          
         std::scoped_lock lock(_database->_writeMutex);

         if (value.size() == 0) {
            if (hasData())
               deleteData();
            return;
         }
         
         Branch branch = getBranch();
         
         
         Data data(value);

         if (branch._dataIndex)
         {
            if (getDataSize() < data.size())
            {
               deleteData();
               branch._dataIndex = 0;
            }
         }
         
         if (branch._dataIndex == 0)
         {
            branch._dataIndex = 
               _database->allocate(data.size());
            setBranch(branch);
         }
         
         _database->setData(branch._dataIndex, data);
         
         
      }
      
      template<typename T>
      void setData(const T& source)
      {
         const char* data = (const char*)&source;
         std::string string(data, sizeof(T));
         setData(string);
         
      }

      void setData(
         const char* source
      )
      {
         setData(std::string(source));
      }
      
      Branch getBranch()
      {
         return
            _database->getBranch(_index);

      }

      const Branch getBranch() const
      {
         return
           _database->getBranch(_index);
      }
      
      void setBranch(const Branch& branch) const
      {
         _database->setBranch(_index, branch);
      }
      
      
      Branch getBranch(Size index)
      {
         return
            _database->getBranch(index);
      }

      const Branch getBranch(Size index)
      const
      {
         return
            _database->getBranch(index);
      }
      
      void deleteData()
      {
         std::scoped_lock lock(_database->_writeMutex);

         Branch branch = getBranch();
         
         if (branch._dataIndex) {
            branch._dataIndex = 0;
            setBranch(branch);
         }
      }
      
      void clear()
      {
         deleteData();
         
         std::scoped_lock lock(_database->_writeMutex);
         
         Branch branch = getBranch();
         
         branch._left = 0;
         branch._right = 0;
         
         setBranch(branch);
         
         
      }
      
      
     
   public:
      
      

      bool operator == (const Path& rhs)
      {
         return (_index == rhs._index);
      }
   
      operator bool ()
      {
         return (_index > Branch::Root);
      }
      
      Index index() const
      {
         return _index;
      }
      
      bool isDeadEnd() const
      {
         Branch branch =
            getBranch();
            
         return branch.isDeadEnd();
      }
      
      template<typename T>
      bool contains(const T& object)
      {
         Contains check(_database, _index);
         
         bool contains =
            check.contains(object);
            
         return contains;
      }
      
      bool contains(const char* value)
      {
          return contains(BString(value));
      }
      
      Database& database()
      {
         return *_database;
      }
      
      Size pageSize() const {
         return _database->pageSize();
      }
   
   
      friend ostream& operator <<
      (ostream& out, const Path& path)
      {
          Variable value = path.getVariable();
          out << value;
          return out;
      }
      
      virtual Variable getVariable() const
      {
         Path path(*this);
         
         BeeFishScript::Variable variable = 
            BeeFishScript::Object{};

         variable["index"] =
            (BeeFishScript::Number)path._index;


         if (path._database) {
            variable["hasData"] = path.hasData();
            if (path.hasData())
               variable["dataSize"] = (BeeFishScript::Integer)path.getDataSize();
            variable["branch"] = path.getBranch().getVariable();
         }
         else
            variable["database"] = null;


         return variable;
     
      }
      
   protected:
      
      static bool readBit(istream& in)
      {
         char c;
         in >> c;
         bool bit = (c != '0');
         return bit;
      }
      
      friend istream& operator >>
      (istream& in, Path& path)
      {
         if (Path::readBit(in))
            path.input(in);
         return in;
      }
      
      void input(istream& in)
      {
         Index bookmark = _index;
         
         // Write the left branch
         if (Path::readBit(in))
         {
            writeBit(false);
            input(in);
            _index = bookmark;
         }
            
         // Write the right branch
         if (Path::readBit(in))
         {
            writeBit(true);
            input(in);
            _index = bookmark;
         }
      }
      
      
      virtual void write(ostream& out)
      {
         write(out, _index);
      }
   
      static void writeBit(ostream& out, bool bit)
      {
         if (bit)
            out << '1';
         else
            out << '0';
      }
      
      void write(ostream& out, Index index)
      {
         Branch branch =
            _database->getBranch(index);
         
         if (branch._left)
         {
            // Write the left branch
            Path::writeBit(out, true);
            write(out, branch._left);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         if (branch._right)
         {
            // Write the right branch
            Path::writeBit(out, true);
            write(out, branch._right);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         return;

      }



   protected:
      
      class Contains :
         public PowerEncoding
      {
      protected:
         Database* _database;
         Index _index;
         bool _contains;
      public:
         Contains
         (
            Database* database,
            Index index
         ) :
            _database(database),
            _index(index)
         {
             Branch branch =
               _database->getBranch(_index);
               
            _contains = !branch.isDeadEnd();
         }
         
         virtual void writeBit(bool bit)
         {
            if (!_contains)
               return;
               
            Branch branch =
               _database->getBranch(_index);
               
            if (!bit && branch._left)
               _index = branch._left;
            else if (bit && branch._right)
               _index = branch._right;
            else
            {
               _index = Branch::Root;
               _contains = false;
            }

            PowerEncoding::writeBit(bit);
         }
         
         virtual bool readBit()
         {
            assert(false);
         }
         
         virtual bool peekBit()
         {
            assert(false);
         }
         
         template <class T>
         bool contains
         (const T& object)
         {
            *this << object;
         
            return _contains;
         }
         
         
      };
      
   };

}
#endif