#ifndef BEE_FISH_DATABASE_LEAST_RECENTLY_USED_CACHE_HPP
#define BEE_FISH_DATABASE_LEAST_RECENTLY_USED_CACHE_HPP

#include <iostream>
#include <list>
#include <unordered_map>
#include <utility> // for std::pair

namespace BeeFishDatabase {

    template<typename Key, typename Value>
    class LeastRecentlyUsedCache {
    private:
        Size _capacity;
        
        // list stores {key, value} pairs
        typedef std::pair<Key, Value> NodeType;
        
        typedef std::list<NodeType> ListType;
        // map stores key -> iterator to list element
        typedef std::unordered_map<
            Key,
            typename ListType::iterator
        >
        MapType;
            
        ListType _list;
        MapType _map;
        std::mutex _mutex;
        
    public:
        LeastRecentlyUsedCache(Size capacity = 100000) : 
            _capacity(capacity)
        {
        }
        
        LeastRecentlyUsedCache& operator = (const LeastRecentlyUsedCache& source)
        {
            _capacity = source._capacity;
            return *this;
        }

        optional<Value> get(Key key)
        {
            std::scoped_lock lock(_mutex);
            
            if (_map.find(key) == _map.end()) {
                return nullopt;
            }
       
            // Move the accessed item to the front (MRU)
            _list.splice(
                _list.begin(),
                _list,
                _map[key]
            );
            
            return _map[key]->second;
        }

        void put(Key key, Value value) {

            std::scoped_lock lock(_mutex);
            
            if (_map.find(key) != _map.end())
            {
                // Key exists, update value and move to front
                _map[key]->second = value;
                _list.splice(
                    _list.begin(),
                    _list,
                    _map[key]
                );
            }
            else
            {
                // New item, add to front
                _list.push_front({key, value});
                _map[key] = _list.begin();

                // Evict the least recently used item if capacity is exceeded
                if (_list.size() > _capacity) {
                    Key lru_key =
                        _list.back().first;
                    _list.pop_back();
                    _map.erase(lru_key);
                }
            }
        }
    };

}

#endif
