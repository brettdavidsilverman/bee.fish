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
#include "PathBase.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;

namespace BeeFishDatabase {

    class Path :
        public PowerEncoding,
        public PathBase
    {
    public:
        Database*  _database  = nullptr;
        SSize      _lockIndex = -1;

    public:

        Path( Database* database = nullptr,
                Index index = Branch::Root ) :
            PowerEncoding(),
            PathBase(index),
            _database(database)
        {
        }

        Path( Database& database,
                Index index = Branch::Root ) :
            PowerEncoding(),
            PathBase(index),
            _database(&database)
        {
        }

        Path(const Path& source) :
            PowerEncoding(),
            PathBase(source._index),
            _database(source._database)
        {
        }
        
        Path( const Path& source,
                Index index) :
            PowerEncoding(),
            PathBase(index),
            _database(source._database)
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
            
            PowerEncoding::readBit();
            
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
            if (bit == 0)
            {
                 goLeft();
            }
            else
            {
                 goRight();
            }
          
            PowerEncoding::writeBit(bit);
          
        }
    
    public:
        
        
    protected:
/*
        bool min(
            Stack& stack,
            Index index
        )
        {
            _index = index;
            bool bit;
            
            while (!isDeadEnd())
            {
                
                
                if (canGoLeft()) {
                    bit = 0;
                    goLeft();
                    
                }
                else if (canGoRight()) {
                    bit = 1;
                    goRight();
                }
                else {
                    break;
                }
                
                stack.push_back(bit);
                
                if (stack.count() == 0)
                    return true;

            }
            
            
            return (stack.count() == 0);
            
        }

        
        bool max(
            Stack& stack,
            Index index
        )
        {
            _index = index;
            bool bit;
            while (!isDeadEnd())
            {
                
                
                if (canGoRight()) {
                    bit = 1;
                    goRight();
                }
                else if (canGoLeft()) {
                    bit = 0;
                    goLeft();
                }
                else
                    break;
                    
                stack.push_back(
                    bit
                );

                if (stack.count() == 0)
                    return true;
                
            }
            
            return (stack.count() == 0);
        }
*/
    public:

        virtual void goLeft()
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
                        _database->getNextIndex(_index);
                    setBranch(branch);
                }
                _database->unlock();
                
            }

            _index = branch._left;
            

        }
        
        virtual void goRight()
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
                        _database->getNextIndex(_index);
                    setBranch(branch);
                }
                _database->unlock();
                
            }

            _index = branch._right;
            
        }
        
        virtual void goUp()
        {
            Branch branch = getBranch();
            Branch parent = getBranch(
                branch._parent
            );
            
            // Reverse the count
            if (parent._left == _index)
                ++_count;
            else
                --_count;

            _index = branch._parent;
            
        }
        
        virtual bool canGoLeft() const
        {
            Branch branch = getBranch();
            
            return (bool)branch._left;
        }
        
        virtual bool canGoRight() const
        {
            Branch branch = getBranch();
            
            return (bool)branch._right;
        }
        
        
        
    public:
        template<typename T>
        Path operator [] (const T& key) const
        {

            Path path(*this);

            path << key;

            path.unlock();

            return path;
        }
 
        Path operator [] (const char* key) const
        {
            BString _key(key);
            return Path::operator[](_key);
        }
        
        Path operator [] (const Path& key) const
        {
            return Path::operator[](key.index());
        }

      
        bool hasData() const
        {
            std::string data = getData();
            
            return (data.size() > 0);
        }
        
        std::string getData() {
             
            Branch branch = getBranch();
            
            if (branch._dataIndex)
            {
                return
                    _database->getData(
                        branch._dataIndex
                    );
            }

            return "";


        }
        
        const std::string getData() const {

            Branch branch = getBranch();
            
            if (branch._dataIndex)
            {
                return
                    _database->getData(
                        branch._dataIndex
                    );
            }

            return "";
            
        }
        
        
        template<typename T>
        void getData(T& destination)
        {
            std::string data = getData();
            destination = *(T*)data.data();
        }

        void getData(std::string& destination)
        {
            destination = getData();
        }

        void getData(BString& destination)
        {
            destination = getData();
        }

        void setData(const std::string& value) {

            Database::ScopedFileLock lock(database());
            
            Branch branch = getBranch(_index);

            std::string current =
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
        
        void setData(
            const char* source
        )
        {
            setData(std::string(source));
        }
        
        void setData(const BString& value)
        {
            setData(value.str());
        }
  
        template<typename T>
        void setData(const T& source)
        {
            const char* bytes = (const char*)&source;
            std::string data(bytes, sizeof(T));
            setData(data);
            
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
            Database::ScopedFileLock lock(database());

            Branch branch = getBranch();
            
            if (branch._dataIndex) {
                branch._dataIndex = 0;
                setBranch(branch);
            }

        }
        
        virtual void clear()
        {
            Database::ScopedFileLock lock(database());

            deleteData();
            
            
            Branch branch = getBranch();
            
            branch._left = 0;
            branch._right = 0;
            
            setBranch(branch);
            
            
        }
        
        
      
    public:
        
        

        bool operator == (const Path& rhs) const
        {
            return (_index == rhs._index);
        }
        
        bool operator != (const Path& rhs) const
        {
            return (_index != rhs._index);
        }
    
        operator bool () const
        {
            return (_index > Branch::Root);
        }
        
        Index index() const
        {
            return _index;
        }
        
        virtual bool isDeadEnd() const
        {
            Branch branch =
                getBranch();
                
            return branch.isDeadEnd();
        }
        
        template<typename T>
        bool contains(const T& object) const
        {
            Contains check(_database, _index);
            
            bool contains =
                check.contains(object);
                
            return contains;
        }
        
        bool contains(const char* value) const
        {
             return contains(BString(value));
        }
        
        bool contains(const Path& key) const
        {
             return contains(key.index());
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
                (BeeFishScript::Integer)_index;

            if (_lockIndex == -1)
                variable["lockIndex"] =
                    (BeeFishScript::Number)(-1);
            else
                variable["lockIndex"] =
                    (BeeFishScript::Integer)_lockIndex;


            if (_database) {
                std::string data = getData();
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
