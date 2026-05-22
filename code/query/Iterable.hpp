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
        JSONDatabase* _database;
        PathBase* _path;
        BeeFishAuthentication::
            Authentication& _auth;
    public:

        Iterable(
            JSONDatabase& database,
            PathBase& path,
            BeeFishAuthentication::
                Authentication& auth
        ) :
            _database(&database),
            _path(path.copy()),
            _auth(auth)
        {
        }
        
        Iterable(const Iterable& source) :
            _database(source._database),
            _path(source._path->copy()),
            _auth(source._auth)
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
                    assert(!iterator._isEnd);
                    ++iterator;
                    if (!iterator._isEnd) {
                        _value = toString(iterator);
                        _jsonPath = jsonPath(iterator);
                        if (
                            (
                                _value +
                                BString("/")
                            ).startsWith(
                                _parentValue + 
                                BString("/")
                            ) 
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
                assert(_iterator);
                assert(!_iterator->_isEnd);
                ++(*_iterator);
                setValue();
                return *this;
            }

            // Postfix increment operator (++)
            Iterator operator++(int) 
            {
                assert(!_iterator->_isEnd);
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
        
        /*
        class ReverseIterator {
        protected:
            typedef BeeFishDatabase::Iterable<JSONPath::Id>::ReverseIterator IdIterator;
            typedef BeeFishDatabase::Iterable<JSONPath::Id> IdIterable;
            IdIterable* _iterable = nullptr;
            IdIterator* _iterator = nullptr;
            JSONDatabase* _database;
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

            // Constructor
            ReverseIterator(
                const Iterable& iterable
            ) :
                _iterable(new IdIterable(*iterable._path)),
                _iterator(new IdIterator(*_iterable)),
                _database(iterable._database)
            {
                setValue();
            }
            
            ReverseIterator()
            {
            }
            
            ReverseIterator(
                const ReverseIterator& source
            )
            {
                _iterable = nullptr;
                    
                if (source._iterator)
                    _iterator = new IdIterator(*source._iterator);
                else
                    _iterator = nullptr;
                    
                _database = source._database;
                _parentValue = source._parentValue;
                _value = source._value;
                _parentPath = source._parentPath;
                _jsonPath = source._jsonPath;
                
            }
            
            virtual ~ReverseIterator()
            {
                if (_iterator)
                    delete _iterator;
                    
                if (_iterable)
                    delete _iterable;
            }
            
            ReverseIterator& operator = (const ReverseIterator& source)
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
                    
                _database = source._database;
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
                        
                   // save();
                    IdIterator iterator = *_iterator;
                    ++iterator;
                    if (!iterator._isEnd) {
                        _value = toString(iterator);
                        _jsonPath = jsonPath(iterator);
                        if (_parentValue.startsWith(_value))
                        {
                            _parentValue = _value;
                            _parentPath = _jsonPath;
                            *_iterator = iterator;
                        }
                        else {
                            _value = _parentValue;
                            _jsonPath = _parentPath;
                          //  restore();
                            break;
                        }
                    }
                    else {
                        _value = _parentValue;
                        _jsonPath = _parentPath;
                       // restore();
                        break;
                    }
                }
                
                
            }
            
            BString toString(IdIterator& iterator) 
            {
                
                return jsonPath(iterator).toString() + BString("/");

            }
            
            JSONPath jsonPath(IdIterator& iterator)
            {
                Path path = _database->objects()[*iterator];
                
                Index index = path.getData<Index>();
                
                return JSONPath(
                    *_database,
                    index
                );
                
                
            }
            
            // Dereference operator (*)
            reference operator*() const
            {
                return _value;
            }

            // Arrow operator (->)
            pointer operator->() const 
            {
                return &_value; 
            }

            // Prefix increment operator (++)
            ReverseIterator& operator++() 
            {
                assert(!_iterator->_isEnd);
                ++(*_iterator);
                return *this;
            }

            // Postfix increment operator (++)
            ReverseIterator operator++(int) {
                ReverseIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator == (
                const ReverseIterator& a,
                const ReverseIterator& b
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
                const ReverseIterator& a,
                const ReverseIterator& b
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
   */
        virtual Iterator begin() 
        {
            return Iterator(*this);
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