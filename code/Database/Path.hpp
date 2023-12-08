#ifndef BEE_FISH__DATABASE__PATH_HPP
#define BEE_FISH__DATABASE__PATH_HPP

#include <iostream>
#include <atomic>
#include "../PowerEncoding/PowerEncoding.hpp"
#include "../Miscellaneous/optional.h"
#include "../Script/Variable.hpp"
#include "Data.hpp"
#include "File.hpp"
#include "Branch.hpp"
#include "DatabaseBase.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {

   class Path :
      public PowerEncoding
   {
   public:
      Database* _database;
      Index     _index;
      
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

      Path( const Path& source,
            Index index) :
         PowerEncoding(),
         _database(source._database),
         _index(index)
      {
      }
   
      Path(const Path& source) :
         PowerEncoding(),
         _database(source._database),
         _index(source._index)
         
      {
         
      }

      virtual ~Path()
      {
      }
      
      virtual void writeBit(bool bit)
      {

         Branch& branch = getBranch();

         if (bit == false)
         {
            if (!branch._left) {
               scoped_lock lock(_database->_mutex);
               if (!branch._left) {
                  branch._left =
                     _database->getNextIndex();
               }
            }
            _index = branch._left;
            
         }
         else
         {
            if (!branch._right) {
               scoped_lock lock(_database->_mutex);
               if (!branch._right) {
                  branch._right =
                     _database->getNextIndex();
               }
            }
            _index = branch._right;
            
         }

         PowerEncoding::writeBit(bit);
         
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
            (string(key));
      }

      Size getDataSize() const
      {
         const Branch& branch =
            getBranch();
            
         if (branch._dataIndex)
         {
            const Data* data =
               _database->getData(
                  branch._dataIndex
               );
               
            if (data)
               return data->size();
         }
         
         return 0;
      }
      
      bool hasData() const
      {
         const Branch& branch =
            getBranch();
         
         if (branch._dataIndex)
         {
            const Data* data =
               _database->getData(
                  branch._dataIndex
               );
               
            if (data)
               return true;
         }

         return false;
      }
      
      Data& getData() {

         Branch& branch =
            getBranch();
            
         if (branch._dataIndex)
         {
            Data* source =
               _database->getData(
                  branch._dataIndex
               );

            return *source;

         }

         assert(false);

      }

      template<typename T>
      void getData(T& destination)
      {
         Data& data = getData();
         destination = *(T*)data.data();
      }


/*
      template<typename T>
      operator T()
      {
         Data& data = getData();
         return (T&)(*data.data());
      }
*/
      void getData(std::string& destination)
      {
         Data& data = getData();
         if (data.size() == 0)
            destination = "";
         else {
            std::string string(
               data.data(),
               data.size()
            );

            destination = string;
         }
      }

      operator string()
      {
         std::string data;
         getData(data);

         if (data.size() == 0)
            return "";

         return data;

      }

      void setData(const void* value, Size size) {
         const string string((const char*)value, size);
         setData(string);
      }
         
      void setData(const std::string& value) {
         Branch& branch = getBranch();
         
         Data* destination = createData(value.size());

         assert(destination);

         if (value.size())
            memcpy(
               destination->data(),
               value.data(),
               value.size()
            );
         
      }

      Data* createData(Size byteSize) 
      {
         Branch& branch = getBranch();
         Data* destination = nullptr;

         if (branch._dataIndex)
         {
            destination =
               _database->getData(
                  branch._dataIndex
               );
         }

         if ( ( destination == nullptr ) || 
              ( destination->size() < byteSize ) )
         {
         
            if (destination)
               deleteData();
            
            Index dataIndex = 
               _database->allocate(byteSize);
               
            Branch& branch =
               getBranch();
               
            branch._dataIndex = dataIndex;
         
            destination =
               _database->getData(
                  branch._dataIndex
               );
            
         }

         destination->_size = byteSize;
         
         return destination;
         
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
      
      Branch& getBranch()
      {
        
         return
            _database->getBranch(_index);
      }

      Branch getBranch() const
      {
         return
            _database->getBranch(_index);
      }
      
      Branch& getBranch(Size index)
      {
         return
            _database->getBranch(index);
      }

      const Branch& getBranch(Size index)
      const
      {
         return
            _database->getBranch(index);
      }
      
      void deleteData()
      {
         Branch& branch = getBranch();

         if (branch._dataIndex) {
            Data* data =
               _database->getData(
                  branch._dataIndex
               );

            memset(
               data->data(),
               0,
               data->size()
            );

            branch._dataIndex = 0;
         }
      }
      
      void clear()
      {
         deleteData();
         Branch& branch = getBranch();
         
         branch._left = 0;
         branch._right = 0;
         
      }
      
      
      virtual bool readBit()
      {
         PowerEncoding::readBit();

         const Branch& branch =
            getBranch(_index);
            
         if (branch._left)
         {
            _index = branch._left;
            return 0;
         }
         else if (branch._right)
         {
            _index = branch._right;
            return 1;
         }
         
         // Path read bit past end of file
         assert(false);
         
      }
      
      virtual bool peekBit()
      {
         const Branch& branch =
            getBranch();
               
         if (branch._left)
            return 0;
         else if (branch._right)
            return 1;
               
         // Path peek bit past end of file
         assert(false);

      }

   public:
         
#include "Stack.hpp"

   protected:

      void min(
         Index index,
         Stack& stack
      ) const
      {
         Branch branch =
            getBranch(index);

         while (!branch.isDeadEnd())
         {
            Path path(*this, index);
            
            bool bit;

            if (branch._left) {
               index = branch._left;
               bit = 0;
            }
            else if (branch._right) {
               index = branch._right;
               bit = 1;
            }

            stack.push_back({new Path(path), bit});

            branch = getBranch(index);
         }
      }

      void max(
         Index index,
         Stack& stack
      ) const
      {

         
         Branch branch =
            getBranch(index);

         while (!branch.isDeadEnd())
         {
            Path path(*this, index);

            bool bit;
         
            if (branch._right) {
               index = branch._right;
               bit = 1;
            }
            else if (branch._left) {
               index = branch._left;
               bit = 0;
            }

            stack.push_back({new Path(path), bit});

            branch = getBranch(index);
         }
      
      }

   public:
      template<typename T>
      T min(
         Stack& stack
      ) const
      {
         min(_index, stack);
         T minimum;
         stack >> minimum;
         assert(stack.count() == 0);
         return minimum;
      }

      template<typename T>
      T max(
         Stack& stack
      ) 
      {
         max(_index, stack);
         T maximum;
         stack >> maximum;
         return maximum;
      }

      template<typename T>
      T min() const {
         Stack stack;
         min(_index, stack);
         T minimum;
         stack >> minimum;
         return minimum;
      }

      template<typename T>
      T max() const {

         Stack stack;
         max(_index, stack);
         T maximum;
         stack >> maximum;
         return maximum;
      }

      template<typename T>
      bool next(Stack& stack, T& value) {
         // Algorithm:
         // Up the tree until first
         // right with a left

         // Take that right, then
         // follow using min algo

         if (isDeadEnd())
            return false;

         if (stack.size() == 0)
         {
            min(_index, stack);
            stack >> value;
            return true;
         }
         
         Branch branch;
         Path* path;
         StackValue entry;
         // Up the tree until first right
         do 
         {
            entry = stack.last();
            path = entry._path;
            branch = path->getBranch();
            stack.pop_back();
         }
         while (stack.size() && 
                ( !(branch._left &&
                     branch._right) ||
                entry._bit == 1) );

         if ( ! (branch._left and
                  branch._right) ||
              (entry._bit == 1) )
            return false;


         assert(branch._left && branch._right && entry._bit == 0);

         // Follow the next min from
         // this right
         stack.push_back({new Path(*path), 1});
         min(branch._right, stack);

         // Get this value
         stack.reset();
         value = T();
         stack >> value;

         return true;

      }

      template<typename T>
      T value() const
      {
         if (isDeadEnd())
            return T();
            
         return min<T>();
      }

      Path& operator=(const Path& rhs)
      { 
         _database = rhs._database;
         _index = rhs._index;
         return *this;
      }
      
      template<typename T>
      T operator=(const T& rhs)
      {
         setData(rhs);
         return rhs;
      }

      string operator=(const char* rhs)
      {
         string value(rhs);
         setData(value);
         return value;
      }
      

      bool operator == (const Path& rhs)
      {
         return (_index == rhs._index);
      }
   
      operator bool ()
      {
         return (_index > Branch::Root);
      }
      
      const Index& index() const
      {
         return _index;
      }
      
      bool isDeadEnd() const
      {
         const Branch& branch =
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
         BeeFishScript::Variable variable = 
            BeeFishScript::Object{};

         variable["index"] =
            (BeeFishScript::Number)path._index;


         if (path._database) {
            variable["hasData"] = path.hasData();
            variable["branch"] = path.getBranch().getVariable();
         }
         else
            variable["database"] = BeeFishScript::Null();


         out << variable;
     
         return out;
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
         bool _isDeadEnd;
         const Database* _database;
         Index _index;
         bool _contains;
      public:
         Contains
         (
            const Database* database,
            Index index
         ) :
            _database(database),
            _index(index)
         {
            const Branch& branch =
               _database->getBranch(_index);
               
            _contains = !branch.isDeadEnd();
         }
         
         virtual void writeBit(bool bit)
         {
            if (!_contains)
               return;
            
            const Branch& branch =
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