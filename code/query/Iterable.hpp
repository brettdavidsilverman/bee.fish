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
            BString _parentValue;
            BString _value;
            JSONPath _parentPath;
            JSONPath _jsonPath;
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
                    
                _parentValue = source._parentValue;
                _value = source._value;
                _parentPath = source._parentPath;
                _jsonPath = source._jsonPath;
            
            }
            
            Iterator(const Iterable& iterable, 
                     const Stack& stack) :
                _container(&iterable),
                _iterable(new IdIterable(*iterable._path)),
               // _iterator(new IdIterator(*_iterable)),
                _iterator(new IdIterator(*_iterable, stack))
            {
                /*
                for (const auto bit : stack)
                {
                    if (bit == 0 && _iterable->canGoLeft())
                        _iterable->goLeft();
                    else if (bit == 1 && _iterable->canGoRight())
                        _iterable->goRight();
                    else {
                        throw runtime_error("Invalid iterator stack");
                    }
                }
                */

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
                    
                _parentValue = source._parentValue;
                _value = source._value;
                _parentPath = source._parentPath;
                _jsonPath = source._jsonPath;
                
                return *this;
            }
            
            
            void setValue()
            {
                
                

                while (!_iterator->_isEnd) {
                    _parentValue = 
                        toString(*_iterator);
                    _parentPath =
                        jsonPath(*_iterator);
                        
                    IdIterator iterator = *_iterator;
    
                    ++iterator;
                    if (!iterator._isEnd) {
                        _value = toString(iterator);
                        _jsonPath = jsonPath(iterator);
                        if (
                            (_value + "/")
                            .startsWith(
                                _parentValue +  "/"
                            ) &&
                            ! _jsonPath.contains("{HTTP}")
                        )
                        {
                            _parentValue = _value;
                            _parentPath = _jsonPath;
                            
                        }
                        else if (_parentValue != "") {
                            _value = _parentValue;
                            _jsonPath = _parentPath;
                            break;
                        }
                    }
                    else if (_parentValue != "") {
                        _value = _parentValue;
                        _jsonPath = _parentPath;
                        break;
                    }
                    
                    *_iterator = iterator;
                }
                
                
            }
            
            BString toString(IdIterator& iterator) 
            {
                return jsonPath(iterator).toString(_container->_auth);
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
            
            JSONPath jsonPath()
            {
                return _jsonPath;
            }
            
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
            const BString& key
        ) const 
        {

            Stack stack = Stack::fromData(key.fromBase64());

            return Iterator(*this, stack);
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