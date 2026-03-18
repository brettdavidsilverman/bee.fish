#ifndef BEE_FISH__DATABASE__PATH_HPP
#define BEE_FISH__DATABASE__PATH_HPP

#include <iostream>
#include <atomic>
#include <fcntl.h>

#include "../power-encoding/power-encoding.h"
#include "../Script/Variable.hpp"
#include "Data.hpp"
#include "File.hpp"
#include "LockFile.hpp"
#include "Branch.hpp"
#include "DatabaseBase.hpp"
#include "PathBase.hpp"

//#undef VERBOSE

namespace BeeFishDatabase {
using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;

    class Path :
        public PowerEncoding,
        public PathBase
    {
    private:
        Database*  _database  = nullptr;
        
        static const Index UNLOCKED = Index(-1);
        
        Index _index = 0;
        Index _savedIndex = 0;
        Index _lockIndex = UNLOCKED;
        
    public:
        
        Path()
        {
        }

        Path( Database& database,
                Index index = Branch::Root ) :
            PowerEncoding(),
            _database(&database),
            _index(index),
            _savedIndex(index)
        {
        }

        Path(const Path& source) :
            PowerEncoding(),
            _database(source._database),
            _index(source._index),
            _savedIndex(_index)
        {
        }
        
        Path( const Path& source,
                Index index) :
            PowerEncoding(),
            _database(source._database),
            _index(index),
            _savedIndex(index)
        {
        }

        virtual ~Path()
        {
            if (locked())
                unlock();
        }
        
        virtual void lock()
        {
            assert(!locked());
            {
                _lockIndex = index();
                _database->lock(_lockIndex);
            }
            
        }
        
        virtual void unlock()
        {
            assert (locked());
            {
                _database->unlock(_lockIndex);
                _lockIndex = UNLOCKED;
            }
        }
        
        bool locked()
        {
            return _lockIndex != UNLOCKED;
        }
        

        Path& operator = (const Path& rhs)
        {
            if (locked()) {
                unlock();
            }
            
            _database = rhs._database;
            _index = rhs._index;
            _savedIndex = rhs._savedIndex;
            
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
        
        virtual Index index() const
        {
            return _index;
        }
        

        virtual void goLeft()
        {

            Branch branch = getBranch();
            
            if (branch._left == 0) 
            {
/*
                if (!locked()) {
                    lock();
                    branch = getBranch();
                }
                
                if (branch._left == 0) 
*/
                {
                    branch._left =
                        _database->getNextIndex(_index);
                }
                
                setBranch(branch);
            }
            
            _index = branch._left;
            
            if (_count > 0) {
                --_count;
                if (locked() && _count == 0)
                    unlock();
            }
                
        }
        
        virtual void goRight()
        {
        
            
            Branch branch = getBranch();

            if (branch._right == 0) 
            {
/*
                if (!locked()) {
                    lock();
                    branch = getBranch();
                }
                
                if (branch._right == 0)
*/
                {
                    branch._right =
                        _database->getNextIndex(_index);
                    setBranch(branch);
                }
            }
            
            _index = branch._right;
            ++_count;
            
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
        
        virtual void save()
        override
        {
            _savedIndex = _index;
        }
        
        virtual void restore()
        override
        {
            _index = _savedIndex;
        }
        
    public:
        
        
        
        bool isRoot()
        {
            return
                index() == _database->rootIndex() ||
                index() == 0;
        }
        
        template<typename T>
        Path parent(T& value)
        {
            assert(!isRoot());
            
            Stack stack;
            Path parent = Path::parent(stack);

            stack.reset();
            
            if (stack.size())
                stack >> value;
                
            return parent;
        }
        
        Path parent() {
            Stack stack;
            return parent(stack);
        }
        
        Path parent(Stack& stack) {
            Index count = 0;
            Index rootIndex = _database->rootIndex();
            Branch branch = getBranch();
            Index index = Path::index();
            assert(!isRoot());
        
            do
            {
                

                Branch parent = _database->getBranch(
                    branch._parent
                );
                
                if (index == parent._left)
                {
                    stack.push_back(0);
                    ++count;
                }
                else if (index == parent._right)
                {
                    stack.push_back(1);
                    --count;
                }
                else
                    assert(false);
    
                index = branch._parent;
                branch = parent;

            }
            while(count > 0 && index != rootIndex && index != 0);
            
            std::reverse(stack.begin(), stack.end());
            stack.reset();

            return Path(*this, index);
        }
        
    public:
        
        
        template<typename T>
        friend
        Path& operator << (Path& path, const T& key)
        {

            PowerEncoding& encoding = path;
            encoding << key;
            
            if (path.locked())
                path.unlock();
                
            return path;
        }
        
        template<typename T>
        Path operator [] (const T& key) const
        {

            Path path(*this);

            path << key;

            return path;
        }
 
        Path operator [] (const char* key) const
        {
            BString _key(key);

            return Path::operator[](_key);
        }
    
        bool hasData() const
        {
            return _database->hasData(index());
        }
        
        Index operator++()
        {
    
            Index count = 1;
            if (hasData())
            {
                getData<Index>(count);
                ++count;
            }
            setData<Index>(count);

            
            return count;
        }
        
        Index operator++(int)
        {
            
            Index existingCount = 0;
            Index count = 1;
            if (hasData())
            {
                getData<Index>(count);
                existingCount = count;
                ++count;
            }
            setData<Index>(count);
            
            return existingCount;
        }
        
        Index operator--()
        {
            
            Index count = 0;
            if (hasData())
            {
                getData<Index>(count);
                if (count > 0)
                    --count;
                else
                    assert(false);
            }
            setData<Index>(count);
        
            return count;
        }
        
        Index operator--(int)
        {

            
            Index existingCount = 0;
            Index count = 1;
            if (hasData())
            {
                getData<Index>(count);
                existingCount = count;
                if (count > 0)
                    --count;
                else
                    assert(false);
            }
            setData<Index>(count);

            return existingCount;
        }
        
        BString getData() {

        
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
        
        const BString getData() const {
            Path path = *this;
            return path.getData();
        }
        
        
        template<typename T>
        void getData(T& destination)
        {
            BString data = getData();
            destination = *(T*)data.data();
        }

        template<typename T = BString>
        void getData(BString& destination)
        {
            destination = getData();
        }
        
        template<typename T>
        bool setData(const T& source)
        {
            const char* bytes = (const char*)&source;
            BString data(bytes, sizeof(T));
            return setData<BString>(data);
            
        }
        
        template<typename T = BString>
        bool setData(const BString& value) {
        
            
            Branch branch = getBranch();

            BString current =
                _database->getData(branch._dataIndex);

            if (value.size() == 0) {
                if (current.size() > 0) {
                    deleteData();
                }
                return true;
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
                return true;
            }
            else if (current != value) {
                _database->setData(branch._dataIndex, value);
                return true;
            }
            
            return false;


        }
        
        template<typename T = const char *>
        bool setData(
            const char* source
        )
        {
            return setData<BString>(BString(source));
        }
        
        
        
        Branch getBranch()
        {
            return
                getBranch(_index);

        }

        const Branch getBranch() const
        {
            return
              _database->getBranch(_index);
        }
        
        void setBranch(const Branch& branch)
        {
            _database->setBranch(_index, branch);
        }
        
        
        Branch getBranch(Index index)
        {
            return
                _database->getBranch(index);

        }

        void deleteData()
        {
            _database->deleteData(_index);

        }
        
        virtual void clear()
        {
            
            Path path = *this;
            
            Branch branch = getBranch();

            if (branch._dataIndex) {
                deleteData();
            }
            
            if (branch._left) {
                _database->deleteBranch(branch._left);
            }
                
            if (branch._right) {
                _database->deleteBranch(branch._right);
            }
            

        }
        

        template<typename T>
        void clear(const T& value)
        {

            Path path = *this;

            
            Stack stack;
            stack << value;
            
            // Get the branches
            Index index = Path::index();
            Index lastIndex = index;
            bool lastBit = true;
            
            for (auto bit : stack)
            {
                Branch branch = _database->getBranch(
                    index
                );
                
                if (branch._left &&
                    branch._right)
                {
                    lastIndex = index;
                    lastBit = bit;
                }
                
                if (bit == 0)
                    index = branch._left;
                else 
                    index = branch._right;
                    
            }
            
            Branch branch = _database->getBranch(lastIndex);
            
            if (lastBit == 0)
            {
                _database->deleteBranch(
                    branch._left
                );
            }
            else {
                _database->deleteBranch(
                    branch._right
                );
            }
            
                
            
        }

    public:
        
        Index count()
        {
            Index count = 0;
            Stack stack;
            while (next(stack))
                ++count;
            
            return count;
        }
        

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
        
        Index pageSize() const {
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


            if (_database) {
                BString data = getData();
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

    PowerEncoding& operator << (PowerEncoding& out, const Path& path)
    {
        out << path.index();
        return out;
    }
}

#endif
