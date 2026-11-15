#ifndef BEE_FISH__QUERY__ITERABLE_HPP
#define BEE_FISH__QUERY__ITERABLE_HPP

#include "../Database/Database.hpp"
#include "../Authentication/authentication.h"
#include "QueryBase.hpp"

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
    const JSONPath::Sort _sort;
    
public:
    typedef BeeFishDatabase::Iterable<BeeFishDatabase::Id> IdIterable;
    typedef IdIterable::Iterator IdIterator;
    
    typedef BeeFishDatabase::Iterable<BeeFishDatabase::Key> KeyIterable;
    typedef KeyIterable::Iterator KeyIterator;
    
    Iterable(
        BeeFishAuthentication::
        Authentication& auth,
        JSONDatabase& database,
        const PathBase& path,
        JSONPath::Sort sort
    ) :
        _auth(auth),
        _database(&database),
        _path(path.copy()),
        _sort(sort)
    {

    }

    Iterable(const Iterable& source) :
        _auth(source._auth),
        _database(source._database),
        _path(source._path->copy()),
        _sort(source._sort)
    {
    }
    
    Iterable(
        BeeFishAuthentication::
            Authentication& auth,
        JSONDatabase& database,
        const BeeFishQuery::Statement& statement
    )
    :
        Iterable(
             auth,
             database,
             statement
                 .expression()
                 .getPath(),
             statement
                 .expression()
                 .sort()
        )
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
    
    BString toString(BeeFishDatabase::Id& id, Index& index) const
    {
        index = JSONPath::getIndexFromId(
            *_database,
            id
        );

        JSONPath path(
            *_database,
            index
        );
        
        return
            path
            .toString(_auth);
    }
    
    BString toString(Key& key, Index& index) const
    {
        key.reset();
        
        index = JSONPath::getIndexFromKey(
            key
        );

        JSONPath path(
            *_database,
            index
        );
        
        return
            path
            .toString(_auth);
    }
    
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

        IdIterable*
            _idIterable = nullptr;
        
        IdIterator* 
            _idIterator = nullptr;
            
        KeyIterable*
            _keyIterable = nullptr;
        
        KeyIterator* 
            _keyIterator = nullptr;
            
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
            _container(&iterable)
        {

            switch (iterable._sort)
            {
            case JSONPath::Sort::Id:
                _idIterable =
                    new IdIterable(
                        *iterable._path
                    );
                    
                _idIterator =
                    new IdIterator(
                         *_idIterable
                    );
                    
                setValueById();
                
                break;
            case JSONPath::Sort::Key:

                _keyIterable =
                    new KeyIterable(
                        *iterable._path
                    );
                    
                _keyIterator =
                    new KeyIterator(
                         *_keyIterable
                    );
                    
                setValueByKey();
                
                break;
            };
            
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
                
            if (source._keyIterator)
                _keyIterator = new KeyIterator(*source._keyIterator);
            else
                _keyIterator = nullptr;
        }
        
        Iterator(
            const Iterable& iterable,
            Index from
        ) :
            _container(&iterable)
            
        {
            
            JSONPath json(*_container->_database, from);
            
            switch (iterable._sort)
            {
            case JSONPath::Sort::Id:
            {
                _idIterable = new IdIterable(*iterable._path);
                
                BeeFishDatabase::Id id = json.id();

                Stack stack;
                stack << id;

                _idIterator =
                    new IdIterator(*_idIterable, stack);

                setValueById();
                break;
            }
            case JSONPath::Sort::Key:
            {
                _keyIterable = new KeyIterable(*iterable._path);
                BeeFishDatabase::Key key = json.toKey();
                
                _keyIterator =
                    new KeyIterator(*_keyIterable, key);

                setValueByKey();
                break;
            }
            };
            
            
        }
        
        virtual ~Iterator()
        {
            if (_idIterator)
                delete _idIterator;

            if (_idIterable)
                delete _idIterable;
                
            if (_keyIterator)
                delete _keyIterator;

            if (_keyIterable)
                delete _keyIterable;
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
            if (_idIterator) 
            {
                ++(*_idIterator);
                setValueById();
            }
            else if (_keyIterator)
            {
                ++(*_keyIterator);
                setValueByKey();
            }
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
                
                
            if (_keyIterator)
                delete _keyIterator;

            if (_keyIterable)
                delete _keyIterable;

            _keyIterable = nullptr;
                
            if (source._keyIterator)
                _keyIterator = new KeyIterator(*source._keyIterator);
            else
                _keyIterator = nullptr;

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
            if (_idIterator)
                return _idIterator->toKey();
            else if (_keyIterator)
                return _keyIterator->toKey();
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
            else if (a._keyIterator)
                aIsEnd = a._keyIterator->_isEnd;
                
            if (b._idIterator)
                bIsEnd = b._idIterator->_isEnd;
            else if (b._keyIterator)
                bIsEnd = b._keyIterator->_isEnd;
                
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
            else if (a._keyIterator)
                aIsEnd = a._keyIterator->_isEnd;
                
            if (b._idIterator)
                bIsEnd = b._idIterator->_isEnd;
            else if (b._keyIterator)
                bIsEnd = b._keyIterator->_isEnd;
                
            return aIsEnd != bIsEnd;

        }

        // By Key
        void setValueByKey()
        {

            _value.clear();
            while (!_keyIterator->_isEnd) {

            
                Key parent =
                    **_keyIterator;

                KeyIterator iterator = *_keyIterator;

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


                *_keyIterator = iterator;
            }


        }

        // By Id
        void setValueById()
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