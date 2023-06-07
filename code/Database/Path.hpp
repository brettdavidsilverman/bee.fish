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
               std::lock_guard<std::mutex> lock(_database->_lock);
               if (!branch._left) {
                  branch._left =
                     _database->getNextIndex(branch._left);
               }
            }
            _index = branch._left;
            
         }
         else
         {
            if (!branch._right) {
               std::lock_guard<std::mutex> lock(_database->_lock);
               if (!branch._right) {
                  branch._right =
                     _database->getNextIndex(branch._right);
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

      Size getDataSize()
      {
         Branch& branch =
            _database->getBranch(_index);
            
         if (branch._dataIndex)
         {
         
            Data* data =
               _database->getData(
                  branch._dataIndex
               );
               
            if (data)
               return data->size();
         }
         
         return 0;
      }
      
      bool hasData()
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
         destination = data;
      }

      template<typename T>
      operator T()
      {
         Data& data = getData(data);
         return (T&)data;
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
         std::string string(
            data.data(),
            data.size()
         );

         return string;
      }

      template<typename T>
      void setData(
         const T& value
      )
      {
         Data data(value);
        
         setData(data);
      }

      void setData(const std::string& value) {
         Branch& branch =
            _database->getBranch(_index);
         
         Data* destination =
            _database->getData(
               branch._dataIndex
            );
               
         if ( ( destination == nullptr ) || 
              ( destination->size() < value.size() ) )
         {
            if (destination)
               deleteData();
            
            Index dataIndex = 
               _database->allocate(value.size());
               
            Branch& branch =
               _database->getBranch(_index);
               
            branch._dataIndex = dataIndex;
         
            destination =
               _database->getData(
                  branch._dataIndex
               );
            
         }

         destination->setSize(value.size());
         
         memcpy(
            destination->data(),
            value.data(),
            destination->size()
         );
         
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
      
      void deleteData()
      {
         Branch& branch =
            _database->getBranch(_index);


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
         Branch& branch =
            _database->getBranch(_index);
         
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
      
      Path& operator=(const Path& rhs)
      { 
         _database = rhs._database;
         _index = rhs._index;
         return *this;
      }
      /*
      Path& operator=(const Index& rhs)
      { 
         _index = rhs;
         return *this;
      }
      */
      template<typename T>
      T operator=(const T& rhs)
      {
         Data data(rhs);
         setData(data);
         return rhs;
      }

      string operator=(const char* rhs)
      {
         string value(rhs);
         setData(value);
         return value;
      }
      
      Data& operator=(Data& rhs)
      {
         setData(rhs);
         return rhs;
      }

      operator Data() {
         Data data;
         getData(data);
         return data;
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
      
      virtual bool isDeadEnd()
      {
         Branch& branch =
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
      
      friend ostream& operator <<
      (ostream& out, Path& path)
      {
         Path::writeBit(out, true);
         path.output(out);
      
         return out;
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
         /*
         Branch branch =
            getBranch(index);
         
         bool lastWasLeft = false;
         
         while (1)
         {
            // go least (bottom left)
            while(branch._left || branch._right)
            {
               if (branch._left)
               {
                  out << '1';
                  branch =
                     getBranch(branch._left);
                  lastWasLeft = true;
               }
               else
               {
                  out << '0';

                  branch =
                     getBranch(branch._right);
                  lastWasLeft = false;
               }
               
            }
            
            // go next least (up and right)
            while(branch._parent)
            {
               if (branch._right &&
                   lastWasLeft)
               {
                  out << '1';
                  
                  // go right
                  branch =
                     getBranch(branch._right);
                  
                  lastWasLeft = false;
                  break;
               }
               
               out << '0';
               
               // go up
               branch = 
                  getBranch(branch._parent);
            }
            
            if (!branch._parent)
               break;
         }
         
         return;
         */
        

        

      }
   public:
   
      template<typename T>
      Path& next(T& destination)
      {
         Encoding::operator >> (destination);
         return *this;
      }
      
      void first(ostream& out)
      {
         Branch branch =
            _database->getBranch(_index);
            
         while (!branch.isDeadEnd())
         {
            if (branch._left)
            {
               out << '0';
               _index = branch._left;
            }
            else
            {
               out << '1';
               _index = branch._right;
            }
            
            branch = 
               _database->getBranch(_index);
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