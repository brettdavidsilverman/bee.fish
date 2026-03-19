#ifndef BEE_FISH_DATABASE_LOCKS_HPP
#define BEE_FISH_DATABASE_LOCKS_HPP

#include "LeastRecentlyUsedCache.hpp"

namespace BeeFishDatabase {
using namespace boost::interprocess;
class AutoUnlockMutex :

    public interprocess_mutex

{
protected:
    bool _locked = false;

public:
    AutoUnlockMutex() :
        interprocess_mutex()
    {
    }

    virtual ~AutoUnlockMutex()
    {
        if (_locked) {
            interprocess_mutex::unlock();
        }
    }

    void lock() {
        interprocess_mutex::lock();
        _locked = true;
    }

    void unlock() {
        if (_locked) {
            interprocess_mutex::unlock();
            _locked = false;
        }
    }

};

typedef AutoUnlockMutex Mutex;

typedef LeastRecentlyUsedCache<Index, Mutex> LocksBase;

class Locks : public LocksBase
{
public:

    Locks(const BString& name, LockFile& lockFile, Index capacity) :
        LeastRecentlyUsedCache(name, lockFile, capacity)
    {
    }

    void lock(Index index)
    {
        {
            _lockFile.lock();
            if (_map->find(index) == _map->end())
            {
               
                evict();

                _list->push_back(index);
                auto it = _list->end();
                --it;
                (*_map)[index].first = it;
                (*_map)[index].second.lock();
                
                _lockFile.unlock();

            }
            else {
                _list->splice(
                    _list->end(),
                    *_list,
                    (*_map)[index].first
                );
                _lockFile.unlock();
                (*_map)[index].second.lock();
            }


        }


    }

    void unlock(Index index)
    {
        LockFile::ScopedFileLock lock(_lockFile);

        if (_map->find(index) != _map->end())
        {

            {
                
                // Move to front ready to be evicted
                _list->splice(
                    _list->begin(),
                    *_list,
                    (*_map)[index].first
                );
            }
            (*_map)[index].second.unlock();

        }
    }

    void evict()
    {
        const Index MIN_SIZE = getPageSize();
        auto segment =
            _sharedMemory
            ->get_segment_manager();

        while (segment->get_free_memory() < MIN_SIZE)
        {

            Index index =
                _list->front();

            (*_map)[index].second.lock();
            
            _map->erase(index);

            _list->pop_front();
        }

    }

};

}

#endif
