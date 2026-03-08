#ifndef BEE_FISH_DATABASE_LEAST_RECENTLY_USED_CACHE_HPP
#define BEE_FISH_DATABASE_LEAST_RECENTLY_USED_CACHE_HPP

#include <iostream>
#include <list>
#include <unordered_map>
#include <utility> // for std::pair

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include "LockFile.hpp"

namespace BeeFishDatabase {
using namespace boost::interprocess;



    template<typename Key, typename Value>
    class LeastRecentlyUsedCache {
    private:
        Index _capacity;
        LockFile& _lockFile;
        BString _name;
        BString _sharedMemoryName;
        
        // list value type
        typedef std::pair<Key, offset_ptr<Value> > NodeType;
        // Define the types for the allocator and the list
        // The allocator needs to know about the segment manager
        typedef managed_shared_memory::segment_manager SegmentManager;
        typedef boost::interprocess::allocator<NodeType, SegmentManager> SharedMemoryListAllocator;
        typedef boost::interprocess::list<NodeType, SharedMemoryListAllocator> SharedMemoryList;
        
        typedef std::pair<const Key, typename SharedMemoryList::iterator> MapValue;
        typedef boost::interprocess::allocator<MapValue, SegmentManager> SharedMemoryMapAllocator;
        typedef boost::interprocess::map<Key, typename SharedMemoryList::iterator, std::less<Key>, SharedMemoryMapAllocator> SharedMemoryMap;

        /*
        // map stores key -> iterator to list element
        typedef std::unordered_map<
            Key,
            typename SharedMemoryList::iterator
        >
        MapType;
            */
        managed_shared_memory* _sharedMemory = nullptr;
        SharedMemoryMap* _map = nullptr;
        SharedMemoryList* _list = nullptr;
        
        
    public:
        LeastRecentlyUsedCache(const BString& name, LockFile& lockFile, Index capacity = 100000) : 
            _capacity(capacity),
            _lockFile(lockFile),
            _name(name)
        {
            
            createSharedMemoryObjects(name);
        }
        
        ~LeastRecentlyUsedCache()
        {
            if (_sharedMemory)
                delete _sharedMemory;
            //shared_memory_object::remove(_sharedMemoryName.c_str());
            
        }
        
        optional<Value> get(Key key)
        {
            LockFile::ScopedFileLock lock(_lockFile);
            if (_map->find(key) == _map->end()) {
                return nullopt;
            }

            
            // Move the accessed item to the front (MRU)
            _list->splice(
                _list->begin(),
                *_list,
                (*_map)[key]
            );
            
            return *((*_map)[key]->second);
        }

        void put(Key key, const Value& value) {

            LockFile::ScopedFileLock lock(_lockFile);
            
            if (_map->find(key) != _map->end())
            {
                // Key exists, update value and move to front
                *( (*_map)[key]->second ) = value;
                _list->splice(
                    _list->begin(),
                    *_list,
                    (*_map)[key]
                );
            }
            else
            {
                //cerr << "(LIST,CAP)" << "(" << _list->size() << "," << _capacity << ")" << endl;

    
                evict();
                
                void* ptrValue = 
                    _sharedMemory
                    ->get_segment_manager()
                    ->allocate(sizeof(Value));
                
                new(ptrValue)Value(value);
                
               // offset_ptr<Value> offsetPtrValue(static_cast<Value*>(ptrValue));


                _list->push_front({key, (Value*)ptrValue});
                (*_map)[key] = _list->begin();
                
            
                
            }
        }
        

        Value& operator[] (Key key)
        {
            LockFile::ScopedFileLock lock(_lockFile);
            
            if (_map->find(key) != _map->end())
            {
                // Move the accessed item to the front (MRU)
                _list->splice(
                    _list->begin(),
                    *_list,
                    (*_map)[key]
                );
            }
            else
            {
                evict();

                // New item, add to front
                void* ptrValue = 
                    _sharedMemory
                    ->get_segment_manager()
                    ->allocate(sizeof(Value));
                
                new(ptrValue)Value();
                
               // offset_ptr<Value> offsetPtrValue(static_cast<Value*>(ptrValue));
                
                _list->push_front({key, (Value*)ptrValue});
                (*_map)[key] = _list->begin();
                
            }
            
            
            return *(*_map)[key]->second;
            
        }
        
        void clear()
        {
LockFile::ScopedFileLock lock(_lockFile);
            
            _list->clear();
            _map->clear();
            
            if (_sharedMemory) {
                delete _sharedMemory;
                _sharedMemory = nullptr;
            }
            
            shared_memory_object::remove(_sharedMemoryName.c_str());
            
            createSharedMemoryObjects(_name);
        }
        
        const BString& sharedMemoryName()
        {
            return _sharedMemoryName;
        }
        
    private:
        
        void createSharedMemoryObjects(const BString& name)
        {
            LockFile::ScopedFileLock lock(_lockFile);
            std::hash<std::string> hasher;
            std::filesystem::path path = _lockFile.filename();
            
            // Since path character '/' isnt allowed
            // this will use the filename
            // hash instead
                
            std::size_t hashedValue = hasher(path.string());
            std::stringstream stream;
            stream << hashedValue;
            _sharedMemoryName = 
                BString(stream.str()) +
                BString(name);
                
            Index memorySize = _capacity * 2.5 * (sizeof(MapValue) + sizeof(NodeType) + sizeof(Value));
            
           // shared_memory_object::remove(_sharedMemoryName.c_str());
            
            _sharedMemory = new
                managed_shared_memory(open_or_create, _sharedMemoryName.c_str(), memorySize);
                        
            _list = 
                _sharedMemory->find_or_construct<SharedMemoryList>("LRUList")
                (_sharedMemory->get_segment_manager());
                
            _map = 
                _sharedMemory->find_or_construct<SharedMemoryMap>("LRUMap")
                (_sharedMemory->get_segment_manager());
            
        }
        
        // Evict the least recently used item if capacity is exceeded
        void evict() {
            while (_list->size() >=  _capacity) 
            {
                Key lru_key =
                    _list->back().first;
                
                Value* pointer =
                    (*_map)[lru_key]->second.get();
                
               // delete pointer;
                
                _sharedMemory->deallocate(
                     pointer
                );
                
                _map->erase(lru_key);
                
                _list->pop_back();
                   // cerr << "(LIST,CAP)" << "(" << _list->size() << "," << _capacity << ")" << endl;
            }
        }
        
    };

}

#endif
