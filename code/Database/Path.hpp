#ifndef BEE_FISH__DATABASE__PATH_HPP
#define BEE_FISH__DATABASE__PATH_HPP

#include <iostream>
#include <atomic>
#include "../PowerEncoding/PowerEncoding.hpp"
#include "../Miscellaneous/optional.h"
#include "Data.hpp"
#include "File.hpp"
#include "Branch.hpp"
#include "DatabaseBase.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {


   template<class Encoding = PowerEncoding>
   class Path :
      public Encoding
   {
   public:
      Database* _database;
      Index     _index;
   public:
   
      Path( Database* database = nullptr,
            Index index = Branch::Root ) :
         Encoding(),
         _database(database),
         _index(index)
      {
      }

      Path( Database& database,
            Index index = Branch::Root ) :
         Encoding(),
         _database(&database),
         _index(index)
      {
      }
   
      Path(const Path& source) :
         Encoding(),
         _database(source._database),
         _index(source._index)
         
      {
         
      }

      virtual ~Path()
      {
      }
      
      virtual void writeBit(bool bit)
      {

         Branch& branch =
            _database->getBranch(_index);

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

         Encoding::writeBit(bit);
         
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
            _database->getBranch(_index);
            
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
         return getDataSize() > 0;
      }
      
      Data& getData() {

         Branch& branch =
            _database->getBranch(_index);
            
         if (branch._dataIndex)
         {
            Data* source =
               _database->getData(
                  branch._dataIndex
               );

            return *source;

         }

         throw runtime_error("Path::getData() No data at this index");

      }

      template<typename T>
      void getData(T& destination)
      {
         Data& data = getData();
         destination = *(T*)data.data();
      }

      template<typename T>
      operator T()
      {
         Data& data = getData();
         return (T&)(*data.data());
      }

      void getData(std::string& destination)
      {
         Data& data = getData();

         std::string string(
            data.data(),
            data.size()
         );

         destination = string;
      }

      operator string()
      {
         Data& data = getData(data);

         return std::string(
            data.data(),
            data.size()
         );

      }

      void setData(const std::string& value) {
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
              ( destination->size() < value.size() ) )
         {
            if (destination)
               deleteData();
            
            Index dataIndex = 
               _database->allocate(value.size());
               
            Branch& branch =
               getBranch();
               
            branch._dataIndex = dataIndex;
         
            destination =
               _database->getData(
                  branch._dataIndex
               );
            
         }

         destination->_size = value.size();
         
         memcpy(
            destination->data(),
            value.data(),
            destination->size()
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

         if (byteSize == 0)
         {
            deleteData();
            return nullptr;
         }
         else if ( ( destination == nullptr ) || 
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

      const Branch& getBranch() const
      {
         return
            _database->getBranch(_index);
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

         Branch& branch =
            _database->getBranch(_index);
            
         if (branch._left)
         {
            _index = branch._left;
            return false;
         }
         else if (branch._right)
         {
            _index = branch._right;
            return true;
         }
         
         throw runtime_error("Path read bit past end of file");
         
      }
      
      virtual bool peekBit()
      {
         Branch& branch =
            _database->getBranch(_index);
               
         if (branch._left)
            return false;
         else if (branch._right)
            return true;
               
         return 0;
      }
      
      void min(
         vector<bool>& stack
      ) const
      {
         return min(_index, stack);
      }

      void max(
         vector<bool>& stack
      ) const
      {
         return max(_index, stack);
      }

      Size min() const {
         PowerEncodingStack stack;
         min(_index, stack);
         Size minimum;
         stack >> minimum;
         return minimum;
      }

      Size max() const {
         PowerEncodingStack stack;
         max(_index, stack);
         Size maximum;
         stack >> maximum;
         return maximum;
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
      
      virtual bool isDeadEnd() const
      {
         const Branch& branch =
            _database->getBranch(_index);
            
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
         out << "{\"path\":" << path._index
             << ", \"hasData\": " <<
                ( path.hasData() ?
                  "true" :
                  "false"
                )
             << ", \"branch\": "
             << path.getBranch()
             << "}";
     
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
      
      
      virtual void output(ostream& out)
      {
         output(out, _index);
      }
   
      static void writeBit(ostream& out, bool bit)
      {
         if (bit)
            out << '1';
         else
            out << '0';
      }
      
      void output(ostream& out, Index index)
      {
         Branch branch =
            _database->getBranch(index);
         
         if (branch._left)
         {
            // Write the left branch
            Path::writeBit(out, true);
            output(out, branch._left);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         if (branch._right)
         {
            // Write the right branch
            Path::writeBit(out, true);
            output(out, branch._right);
         }
         else
            // Deadend, close the branch
            Path::writeBit(out, false);
            
         return;

      }

      void min(
         Index index,
         vector<bool>& stack
      ) const
      {
         Branch branch =
            _database->getBranch(index);
         
         if (branch._left)
         {
            stack.push_back(false);
            min(branch._left, stack);
         }
         else if (branch._right)
         {
            stack.push_back(true);
            min(branch._right, stack);
         }
         
      }

      void max(
         Index index,
         vector<bool>& stack
      ) const
      {
         Branch branch =
            _database->getBranch(index);
         
         if (branch._right)
         {
            stack.push_back(true);
            max(branch._right, stack);
         }
         else if (branch._left)
         {
            stack.push_back(false);
            max(branch._left, stack);
         }
         
      }

   protected:
      
      class Contains :
         public Encoding
      {
      protected:
         bool _isDeadEnd;
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
            Branch& branch =
               _database->getBranch(_index);
               
            _contains = !branch.isDeadEnd();
         }
         
         virtual void writeBit(bool bit)
         {
            if (!_contains)
               return;
            
            Branch& branch =
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

            Encoding::writeBit(bit);
         }
         
         virtual bool readBit()
         {
            throw logic_error("readBit() not implemented.");
         }
         
         virtual bool peekBit()
         {
            throw logic_error("peekBit() not implemented.");
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