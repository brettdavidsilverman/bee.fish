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
    typedef BeeFishDatabase::Iterable<JSONPath::Id>::Iterator IdIterator;
    typedef BeeFishDatabase::Iterable<JSONPath::Id> IdIterable;
    
    Iterable(
        BeeFishAuthentication::
        Authentication& auth,
        JSONDatabase& database,
        PathBase& path

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

    // Defined below
    Index count()
    {
        Index count = 0;
        Stack child;
        
        
        IdIterable iterable(*_path);
        IdIterator it(iterable);

        while (!it._isEnd) {

            Stack parent = *it;

            IdIterator
                iterator = it;

            ++iterator;
            if (!iterator._isEnd)
            {

                child = *iterator;

                if (startsWith(
                            parent,
                            child
                        )
                   )
                {
                    parent = child;
                }
                else if (isOurs(_auth, parent))
                {
                    child = parent;
                    ++count;
                }
            }
            else if (isOurs(_auth, parent)) {
                child = parent;
                ++count;
            }
            
            it = iterator;

        }

        return count;
    }

    static bool isOurs(
        BeeFishAuthentication::Authentication& auth,
        Stack& key
    )
    {
        key.reset();
        if (!key.peekBit())
            return true;

        key.readBit();

        bool next;
        key >> next;

        if (!next)
            return true;

        Type type;
        key >> type;

        if (type != Type::USER)
            return true;

        BString userId;
        key >> userId;

        return userId ==
               auth.userId();
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

    class Iterator {
    protected:
        
        const Iterable* _container = nullptr;
        IdIterable* _iterable = nullptr;
        IdIterator* _iterator = nullptr;
        BString _value;
        Index _index;
    public:

        // Iterator traits (required for STL compatibility in C++17 and earlier)
        using iterator_category = std::forward_iterator_tag;
        using value_type        = BString;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const BString*;
        using reference         = const BString&;


        Iterator(
            const Iterable& iterable
        ) :
            _container(&iterable),
            _iterable(new IdIterable(*iterable._path)),
            _iterator(new IdIterator(*_iterable))
        {
            setValue();
        }

        Iterator(
            const Iterator& source
        ) :
            _container(source._container)
        {
            _iterable = nullptr;

            if (source._iterator)
                _iterator = new IdIterator(*source._iterator);
            else
                _iterator = nullptr;

            _value = source._value;
            _index = source._index;

        }

        Iterator(const Iterable& iterable,
                 Index from) :
            _container(&iterable),
            _iterable(new IdIterable(*iterable._path))
        {
            JSONPath json(*_container->_database, from);
            JSONPath::Id id = json.id();

            Stack stack;
            stack << id;

            _iterator =
                new IdIterator(*_iterable, stack);

            setValue();
        }


        Iterator()
        {
        }

        virtual ~Iterator()
        {
            if (_iterator)
                delete _iterator;

            if (_iterable)
                delete _iterable;
        }

        Iterator& operator = (const Iterator& source)
        {

            if (_iterator)
                delete _iterator;

            if (_iterable)
                delete _iterable;

            _iterable = nullptr;

            if (source._iterator)
                _iterator = new IdIterator(*source._iterator);
            else
                _iterator = nullptr;

            _container = source._container;
            _value = source._value;
            _index = source._index;

            return *this;
        }


        void setValue()
        {
            _value.clear();
            Stack child;

            while (!_iterator->_isEnd) {

                Stack parent =
                    **_iterator;

                IdIterator iterator = *_iterator;

                ++iterator;
                if (!iterator._isEnd)
                {

                    child = *iterator;

                    if (Iterable::startsWith(
                                parent,
                                child
                            )
                       )
                    {
                        parent = child;
                    }
                    else if (
                        Iterable::isOurs(
                            _container->_auth,
                            parent
                        )
                    )
                    {
                        child = parent;
                        _value = toString(child);
                        break;
                    }
                }
                else if (
                    Iterable::isOurs(
                        _container->_auth,
                        parent
                    )
                )
                {
                    child = parent;
                    _value = toString(child);
                    break;
                }


                *_iterator = iterator;
            }


        }


        BString toString(Stack& key)
        {
            key.reset();
            BString string =
                JSONPath::keyToString(
                    _container->_auth,
                    key,
                    _index
                );
                
        //    BeeFishWeb::URL url(string);
            

//if (url.path() == "/")
//    return "";
                
                       
            JSONPath path(
                *_container
                ->_database, 
                _index
            );
        
return path.toString(_container->_auth);

        }

        JSONPath jsonPath(IdIterator& iterator)
        {
            Path path = _container->_database->objects()[*iterator];

            Index index = path.getData<Index>();

            return JSONPath(
                       *_container->_database,
                       index
                   );


        }

        Index index() const
        {
            return _index;
        }

        /*
        JSONPath jsonPath()
        {
            return _jsonPath;
        }
        */
        BString toKey()
        {
            return _iterator->toKey();
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
            ++(*_iterator);
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

        friend bool operator == (
            const Iterator& a,
            const Iterator& b
        )
        {
            bool aIsEnd = true;
            bool bIsEnd = true;

            if (a._iterator)
                aIsEnd = a._iterator->_isEnd;

            if (b._iterator)
                bIsEnd = b._iterator->_isEnd;

            return aIsEnd == bIsEnd;
        }

        friend bool operator != (
            const Iterator& a,
            const Iterator& b
        )
        {

            bool aIsEnd = true;
            bool bIsEnd = true;

            if (a._iterator)
                aIsEnd = a._iterator->_isEnd;

            if (b._iterator)
                bIsEnd = b._iterator->_isEnd;

            return aIsEnd != bIsEnd;
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