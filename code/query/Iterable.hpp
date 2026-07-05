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
        
        virtual ~Iterable()
        {
            delete _path;
        }
        
        class Iterator {
        protected:
            typedef BeeFishDatabase::Iterable<JSONPath::Id>::Iterator IdIterator;
            typedef BeeFishDatabase::Iterable<JSONPath::Id> IdIterable;
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
                
                while (!_iterator->_isEnd) {
                    
                    BString parentValue =
                        toString(*_iterator);
                    
                    IdIterator iterator = *_iterator;
    
                    ++iterator;
                    if (!iterator._isEnd) {
                        
                        _value = toString(iterator);
                        
                            
                        if (
                            (_value + BString("/")).startsWith(
                                parentValue + BString("/")
                            )
                            
                        )
                        {
                            parentValue = _value;
                            
                        }
                        else if (parentValue != "") {
                            _value = parentValue;
                            break;
                        }
                    }
                    else if (parentValue!= "") {
                        _value = parentValue;
                        break;
                    }
                    
                    
                    *_iterator = iterator;
                }
                
                if (!_iterator->_isEnd)
                {
                    Path path =
                        _container
                        ->_database
                        ->objects()[**_iterator];
                
                    _index = path.getData<Index>();
                
                    _value += 
                        BString("?index=") +
                        BString(to_string(_index));
                }
                
                
            }
            
            BString toString(IdIterator& iterator) 
            {
                Stack& stack = iterator._stack;
                stack.reset();
                BString key;
                stack >> key;
            
                BString value =
                    JSONPath::keyToString(
                        *_container->_database,
                        _container->_auth,
                        key
                    );
                
                if (value.contains("{http}"))
                {
                    value = value.substr(
                        0,
                        value.find("{http}")
                    );
                }
                
                return value;
                
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