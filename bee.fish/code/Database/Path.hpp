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
#include "LockFile.hpp"
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
      Database* _database  = nullptr;
      Index     _index     = 0;
      ssize_t   _lockIndex = -1;
   public:

      Path( Database* database = nullptr,
            Index index = Branch::Root ) :
         PowerEncoding(),
         _database(database),
         _index(index)
      {
      }

      Path( Database& database,
            Index index = Branch::Root ) :
         PowerEncoding(),
         _database(&database),
         _index(index)
      {
      }

      Path(const Path& source) :
         PowerEncoding(),
         _database(source._database),
         _index(source._index)
      {
      }
      
      Path( const Path& source,
            Index index) :
         PowerEncoding(),
         _database(source._database),
         _index(index)
      {
      }

      virtual ~Path()
      {
         unlock();
      }

      virtual void unlock() {
         if(_lockIndex >= 0) {
            _database->unlockBranch(_lockIndex);
            _lockIndex = -1;
         }
      }

      Path& operator=(const Path& rhs)
      {
         _database = rhs._database;
         _index = rhs._index;
         _lockIndex = -1;

         return *this;
      }
      
      virtual bool readBit()
      {

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
         
         if (bit)
            ++PowerEncoding::_count;
         else
            --PowerEncoding::_count;

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
         if (bit == 0)
         {
            _index = goLeft();
         }
         else
         {
            _index = goRight();
         }
        
         PowerEncoding::writeBit(bit);
        
      }
      
   protected:

      Index goLeft()
      {

         Branch branch = getBranch();

         if (branch._left == 0) 
         {

            if (_lockIndex < 0) {
               branch = _database->lockBranch(_index);
               _lockIndex = _index;
            }

            _database->lock();

            branch = getBranch();


            if (branch._left == 0)
            {
               branch._left =
                  _database->getNextIndex();
               setBranch(branch);
            }
            _database->unlock();
            
         }

         return branch._left;
      }
      
      Index goRight()
      {
         Branch branch = getBranch();

         if (branch._right == 0) 
         {

            if (_lockIndex < 0) {
               branch = _database->lockBranch(_index);
               _lockIndex = _index;
            }

            _database->lock();
            
            branch = getBranch();


            if (branch._right == 0)
            {
               branch._right =
                  _database->getNextIndex();
               setBranch(branch);
            }
            _database->unlock();
            
         }

         return branch._right;
      }
      
   public:
      template<typename T>
      Path operator [] (const T& key)
      {

         Path path(*this);

//         Database::ScopedFileLock lock(_database);
         
         path << key;

         path.unlock();

         return path;
      }
 
      Path operator [] (const char* key)
      {
         std::string _key(key);
         return Path::operator[](_key);
      }

     
      bool hasData() const
      {
         Data data = getData();
         
         return (data.size() > 0);
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

         return Data();


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

         return Data();


      }
      
      
      template<typename T>
      void getData(T& destination)
      {
         Data data = getData();
         destination = *(T*)data.data();
      }

      void getData(std::string& destination)
      {
         const Data data = getData();
         if (data.size())
            destination = std::string(
               data.data(),
               data.size()
            );
         else
            destination = "";
      }

      void setData(const std::string& value) {
         Data source(value);
         setData(source);
      }

      void setData(const Data& value) {

         scoped_lock threadLock(_database->_mutex);

         Database::ScopedFileLock lock(_database);
         
         Branch branch = getBranch(_index);

         Data current =
            _database->getData(branch._dataIndex);

         if (value.size() == 0) {
            if (current.size() > 0) {
               deleteData();
            }
            return;
         }
         
         
         if (branch._dataIndex && current.size() < value.size())
         {
            deleteData();
            branch._dataIndex = 0;
         }
         
         if (branch._dataIndex == 0)
         {
            branch._dataIndex = 
               _database->allocate(value);
            setBranch(branch);
         }
         else if (current != value) {
            _database->setData(branch._dataIndex, value);
         }

         
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
      
      
      Branch getBranch(Size index) const
      {
         return
            _database->getBranch(index);
      }

      void deleteData()
      {
         Database::ScopedFileLock lock(_database);

         Branch branch = getBranch();
         
         if (branch._dataIndex) {
            branch._dataIndex = 0;
            setBranch(branch);
         }

      }
      
      void clear()
      {
         Database::ScopedFileLock lock(_database);

         deleteData();
         
         
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
         BeeFishScript::Variable variable = 
            BeeFishScript::Object{};

         variable["index"] =
            (BeeFishScript::Number)_index;

         variable["lockIndex"] =
            (BeeFishScript::Number)_lockIndex;


         if (_database) {
            Data data = getData();
            variable["hasData"] = (data.size() > 0);
            if (data.size())
            {
               variable["dataSize"] = (BeeFishScript::Integer)data.size();
            }
            variable["branch"] = getBranch().getVariable();
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
            return false;
         }
         
         virtual bool peekBit()
         {
            assert(false);
            return false;
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
