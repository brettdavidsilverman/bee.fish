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
        BString _filename;
        
        // list stores {key, value} pairs
        typedef std::pair<Key, Value> NodeType;
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
        managed_shared_memory* _sharedMemory;
        SharedMemoryMap* _map;
        SharedMemoryList* _list;
        LockFile& _lockFile;
        
    public:
        LeastRecentlyUsedCache(LockFile& lockFile, Index capacity = 100000) : 
            _capacity(capacity),
            _lockFile(lockFile)
        {
            
            std::hash<std::string> hasher;

            // Since path character '/' isnt allowed
            // this will use the filename
            // hash instead
            std::size_t hashedValue = hasher(lockFile.filename());
            std::stringstream stream;
            stream << hashedValue;
            _filename = stream.str();
//#warning need to factor in size of shared memory
            Index memorySize = _capacity * 4 * (sizeof(MapValue) + sizeof(NodeType));
            
            shared_memory_object::remove(_filename.c_str());
            
            _sharedMemory = new
                managed_shared_memory(open_or_create, _filename.c_str(), memorySize);
                        
            _list = 
                _sharedMemory->find_or_construct<SharedMemoryList>("LRUList")
                (_sharedMemory->get_segment_manager());
                
            _map = 
                _sharedMemory->find_or_construct<SharedMemoryMap>("LRUMap")
                (_sharedMemory->get_segment_manager());
            
        }
        
        ~LeastRecentlyUsedCache()
        {
            delete _sharedMemory;
            shared_memory_object::remove(_filename.c_str());
        }
        
        LeastRecentlyUsedCache& operator = (const LeastRecentlyUsedCache& source)
        {
            _capacity = source._capacity;
            _filename = source._filename;

            return *this;
        }

        optional<Value> get(Key key)
        {
            if (_map->find(key) == _map->end()) {
                return nullopt;
            }

            // Move the accessed item to the front (MRU)
            _list->splice(
                _list->begin(),
                *_list,
                (*_map)[key]
            );
            
            return (*_map)[key]->second;
        }

        void put(Key key, Value value) {

            if (_map->find(key) != _map->end())
            {
                // Key exists, update value and move to front
                (*_map)[key]->second = value;
                _list->splice(
                    _list->begin(),
                    *_list,
                    (*_map)[key]
                );
            }
            else
            {
                //cerr << "(LIST,CAP)" << "(" << _list->size() << "," << _capacity << ")" << endl;

    
                // Evict the least recently used item if capacity is exceeded
                while (_list->size() >  _capacity) 
                {
                    Key lru_key =
                        _list->back().first;
                    _list->pop_back();
                    _map->erase(lru_key);
                   // cerr << "(LIST,CAP)" << "(" << _list->size() << "," << _capacity << ")" << endl;
                    
                }

                // New item, add to front
                _list->push_front({key, value});
                (*_map)[key] = _list->begin();
                
            
                
            }
        }
    };

}

#endif
