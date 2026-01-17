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
            
        ListType _cache_list;
        MapType _cache_map;

    public:
        LeastRecentlyUsedCache(Size capacity) : 
            _capacity(capacity)
        {
        }

        optional<Value> get(Key key)
        {
            if (_cache_map.find(key) == _cache_map.end()) {
                return nullopt;
            }
       
            // Move the accessed item to the front (MRU)
            _cache_list.splice(
                _cache_list.begin(),
                _cache_list,
                _cache_map[key]
            );
            
            return _cache_map[key]->second;
        }

        void put(Key key, Value value) {
            if (_cache_map.find(key) != _cache_map.end())
            {
                // Key exists, update value and move to front
                _cache_map[key]->second = value;
                _cache_list.splice(
                    _cache_list.begin(),
                    _cache_list,
                    _cache_map[key]
                );
            }
            else
            {
                // New item, add to front
                _cache_list.push_front({key, value});
                _cache_map[key] = _cache_list.begin();

                // Evict the least recently used item if capacity is exceeded
                if (_cache_list.size() > _capacity) {
                    Key lru_key =
                        _cache_list.back().first;
                    _cache_list.pop_back();
                    _cache_map.erase(lru_key);
                }
            }
        }
    };

}

#endif
