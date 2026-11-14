#ifndef BEE_FISH__QUERY__ITERABLE_HPP
#define BEE_FISH__QUERY__ITERABLE_HPP

#include "../Database/Database.hpp"
#include "../Authentication/authentication.h"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;
using namespace BeeFishAuthentication;

class Iterable
{
protected:
    BeeFishAuthentication::
    Authentication& _auth;
    JSONDatabase* _database;
    PathBase* _path;

public:
    typedef BeeFishDatabase::Iterable<BeeFishDatabase::Id> IdIterable;
    typedef IdIterable::Iterator IdIterator;
    
    
    Iterable(
        BeeFishAuthentication::
        Authentication& auth,
        JSONDatabase& database,
        const PathBase& path

    ) :
        _auth(auth),
        _database(&database),
        _path(path.copy())

    {
    }

    Iterable(const Iterable& source) :
        _auth(source._auth),
        _database(source._database),
        _path(source._path->copy())
    {
    }

/*
    Index count()
    {
        Index count = 0;

        IdIterable iterable(*_path);
        IdIterator it(iterable);

        while (!it._isEnd) {

            Stack parent = *it;

            IdIterator
                iterator = it;

            ++iterator;
            if (!iterator._isEnd)
            {

                Stack child = *iterator;

                if (!startsWith(
                            parent,
                            child
                        )
                   )
                {
                    ++count;
                }
                
            }
            else
            {
                ++count;
            }
            
            it = iterator;

        }

        return count;
    }
    
*/
    Index countQuick()
    {
        Index count = 0;

        IdIterable iterable(*_path);
        IdIterator it(iterable);
        while (!it._isEnd)
        {
            it++;
            count++;
        }
        
        return count;
    }
    
    Index count()
    {
        Index count = 0;
        IdIterable iterable(*_path);
        IdIterator it(iterable);
        while (!it._isEnd)
        {
            Index parent =
                JSONPath
                ::getIndexFromId(
                    *_database,
                    *it
                );

            IdIterator iterator = it;

            ++iterator;

            if (!iterator._isEnd)
            {

                Index child =
                    JSONPath
                    ::getIndexFromId(
                        *_database,
                        *iterator
                    );

                if (!isChild(parent, child))
                {
                    count++;
                }
            }
            else
            {
                count++;
            }


            it = iterator;
        }
        
        return count;
    }
    
    bool isChild(Index parent, Index child)
    {
        Path path(*_database, child);
        while (!path.isRoot())
        {
            if (path.index() == parent)
                return true;
                
            Index index = 
                path
                .getBranch()
                ._parent;
                
            path = JSONPath(
                *_database,
                index
            );
        }
        
        return false;
    }
    
    BString toString(BeeFishDatabase::Id& key, Index& index) const
    {
        JSONPath path(
            *_database,
            key
        );
        
        index = path.index();
        
        return
            path
            .toString(_auth);
    }
    
    BString toString(Stack& key, Index& index) const
    {
        key.reset();

        JSONPath path(
            *_database,
            key
        );
        
        index = path.index();
        
        return
            path
            .toString(_auth);
    }
/*
    static bool startsWith(
        Stack& parent,
        Stack& child
    )
    {

        parent.reset();
        child.reset();

        bool bit = parent.readBit();
        if (child.peekBit() != bit)
            return false;
        child.readBit();


        bool next;
        parent >> next;

        if (!child.contains(next))
            return false;

        child >> next;


        while (next)
        {

            
            Index order;
            parent >> order;
            
            if (!child.contains(order))
                return false;
                    
            child >> order;

            Type type;
            parent >> type;

            if (!child.contains(type))
                return false;
                
            child >> type;

            if (type == Type::INTEGER)
            {
                Index index;
                parent >> index;
                if (!child.contains(index))
                    return false;
                child >> index;
            }
            else
            {
                BString value;

                parent >> value;

                if (!child.contains(value))
                    return false;

                child >> value;
            }

            parent >> next;

            if (!next)
                break;

            if (!child.contains(next))
                return false;

            child >> next;

        }

        return true;
    }
*/

    virtual ~Iterable()
    {
        delete _path;
    }
    
    class Iterator
    {
    protected:
        const Iterable* _container = nullptr;
        BString _value;
        Index _index;
        
        typedef typename BeeFishDatabase::Iterable<BeeFishDatabase::Id> IdIterable;
        typedef typename IdIterable::Iterator IdIterator;
    
        IdIterable*
            _idIterable = nullptr;
        
        IdIterator* 
            _idIterator = nullptr;
    public:
        
        // Iterator traits (required for STL compatibility in C++17 and earlier)
        using iterator_category = std::forward_iterator_tag;
        using value_type        = BString;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const BString*;
        using reference         = const BString&;
        
        Iterator()
        {
        }
        
        Iterator(const Iterable* container)
        :
            _container(container)
        {
        }
        
        Iterator(
            const Iterable& iterable
        ) :
            _container(&iterable),
            _idIterable(new IdIterable(*iterable._path)),
            _idIterator(new IdIterator(*_idIterable))
        {
            setValue();
        }
        
        Iterator(const Iterator& source)
        :
            _container(source._container),
            _value(source._value),
            _index(source._index)
        {
            _idIterable = nullptr;
                 
            if (source._idIterator)
                _idIterator = new IdIterator(*source._idIterator);
            else
                _idIterator = nullptr;
        }
        
        Iterator(
            const Iterable& iterable,
            Index from
        ) :
            _container(&iterable),
            _idIterable(new IdIterable(*iterable._path))
        {
            JSONPath json(*_container->_database, from);
            BeeFishDatabase::Id id = json.id();

            Stack stack;
            stack << id;

            _idIterator =
                new IdIterator(*_idIterable, stack);

            setValue();
        }
        
        virtual ~Iterator()
        {
            if (_idIterator)
                delete _idIterator;

            if (_idIterable)
                delete _idIterable;
        }
        
        
        Index index() const
        {
            return _index;
        }
        
        // Dereference operator (*)
        reference operator*() const
        {
            return _value;
        }

        pointer operator->() const
        {
            return &_value;
        }
        
        // Prefix increment operator (++)
        Iterator& operator++() {
            ++(*_idIterator);
            setValue();
            return *this;
        }

        // Postfix increment operator (++)
        Iterator operator++(int)
        {
            Iterator tmp = *this;

            ++(*this);
            return tmp;
        }
        
        Iterator& operator = (const Iterator& source)
        {

            if (_idIterator)
                delete _idIterator;

            if (_idIterable)
                delete _idIterable;

            _idIterable = nullptr;
                
            if (source._idIterator)
                _idIterator = new IdIterator(*source._idIterator);
            else
                _idIterator = nullptr;

            _container = source._container;
            _value = source._value;
            _index = source._index;

            return *this;
        }
        
        JSONPath jsonPath(IdIterator& iterator)
        {
            Index index = 
                JSONPath::getIndexFromId(
                    *_container->_database,
                    *iterator
                );

            return 
                JSONPath(
                    *_container
                    ->_database,
                    index
                );
                
        }

        BString toKey()
        {
            return _idIterator->toKey();
        }
        
        friend bool operator == (
            const Iterator& a,
            const Iterator& b
        )
        {
            bool aIsEnd = true;
            bool bIsEnd = true;

            if (a._idIterator)
                aIsEnd = a._idIterator->_isEnd;

            if (b._idIterator)
                bIsEnd = b._idIterator->_isEnd;

            return aIsEnd == bIsEnd;

        }

        friend bool operator != (
            const Iterator& a,
            const Iterator& b
        )
        {

            bool aIsEnd = true;
            bool bIsEnd = true;

            if (a._idIterator)
                aIsEnd = a._idIterator->_isEnd;

            if (b._idIterator)
                bIsEnd = b._idIterator->_isEnd;

            return aIsEnd != bIsEnd;

        }

/*
// By Key
        void setValue(int)
        {
            _value.clear();
            

            while (!_iterator->_isEnd) {

                Key parent =
                    **_iterator;

                IdIterator iterator = *_iterator;

                ++iterator;

                if (!iterator._isEnd)
                {

                    Key child = *iterator;

                    if (!Iterable::startsWith(
                            parent,
                            child
                        )
                    )
                    {
                        _value =
                            _container
                            ->toString(parent, _index);
                            
                        break;
                    }
                }
                else
                {
                    _value =
                        _container
                        ->toString(parent, _index);
                        
                    break;
                }


                *_iterator = iterator;
            }


        }
*/
        // By Id
        void setValue()
        {
            _value.clear();
            Index index;
            while (!_idIterator->_isEnd) {

                BeeFishDatabase::Id parent =
                    **_idIterator;
                    
                BString parentValue =
                    _container
                    ->toString(parent, index);
                    
                IdIterator iterator = *_idIterator;

                ++iterator;

                if (!iterator._isEnd)
                {

                    BeeFishDatabase::Id child = *iterator;
                    BString childValue =
                        _container
                        ->toString(child, index);
                        
                    if (!childValue
                         .startsWith(parentValue)
                    )
                    {
                        _value = parentValue;
                        _index = index;
                        break;
                    }
                }
                else
                {
                    _value = parentValue;
                    _index = index;
                    break;
                }


                *_idIterator = iterator;
            }


        }
        
    };

    virtual Iterator begin()
    {
        
        return Iterator(*this);
    }

    virtual Iterator begin(
        Index from
    ) const
    {
        return Iterator(*this, from);
    }


    // Points one past the last element
    Iterator end()
    {
        return Iterator();
    }

    /*
    virtual ReverseIterator rbegin()
    {
        return ReverseIterator(*this);
    }

    // Points one past the last element
    ReverseIterator rend()
    {
        return ReverseIterator();
    }
    */
};

}
#endif