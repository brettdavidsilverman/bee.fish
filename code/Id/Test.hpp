#ifndef BEE_FISH_ID__TEST_HPP
#define BEE_FISH_ID__TEST_HPP
#include <mutex>
#include <thread>

#include "Id.hpp"
#include "../test/test.h"

using namespace BeeFishTest;

namespace BeeFishId
{
inline bool testTimestamp();
inline bool testTimestamps();
inline bool testId();
inline bool testIds();

inline bool test()
{

    bool ok = true;
    ok = ok && testTimestamp();
    ok = ok && testTimestamps();
    ok = ok && testId();
    ok = ok && testIds();

    if (ok)
        cout << "SUCCESS" << endl;
    else
        cout << "FAIL" << endl;

    return ok;
}

inline bool testTimestamp()
{
    cout << "Timestamp" << endl;

    bool ok = true;

    Stack bits;
    Timestamp timestamp;

    bits << timestamp;

    bits.reset();
    Timestamp check;
    bits >> check;

    ok = testValue(
             "Timestamp equal",
             (check == timestamp)
         );

    Timestamp timestamp1;
    Timestamp timestamp2;

    Stack bits1;
    bits1 << timestamp1;


    Stack bits2;
    bits2 << timestamp2;
    
    Timestamp timestamp3;
    Timestamp timestamp4;

    bits1.reset();
    bits1 >> timestamp3;

    bits2.reset();
    bits2 >> timestamp4;

    ok = testValue(
             "Timestamp 1,3 equal",
             (timestamp1 == timestamp3)
    );
    
    return ok;

}

inline bool testTimestamps()
{
    cout << "Testing large array of timestamps " << endl;
    const int size = 1000;
    std::mutex mutex;
    map<std::string, int> map;
    auto loadMap =
    [size, &map, &mutex]() {
        vector<Timestamp> array(size);
        std::scoped_lock lock(mutex);
        for (Timestamp& timestamp : array)
        {
            auto it = map.find(timestamp.toData());
            if (it != map.end()) {
                cerr << "Duplicate " << timestamp << endl;
            }
            map[timestamp.toData()] = 0;
        }
    };

    
    std::thread threads[] =
    {
        std::thread(loadMap),
        std::thread(loadMap)
    };
    
    threads[0].join();
    threads[1].join();
    
    cerr << "Timestamp " << map.size() << endl;
    
    bool ok = testValue(
        "Timestamps has all with no duplicates",
        map.size() == size * 2
    );

    BeeFishMisc::outputSuccess(ok);

    return ok;

}


inline bool testId()
{
    cout << "Id" << endl;

    bool ok = true;

    Id id("bee");


    Id id2 = Id::fromKey(
                 id.key()
             );

    ok &= testResult(
              "Id from key",
              (id2._name == "bee")
          );

    cout << endl;

    return ok;
}

inline bool testIds()
{
    cout << "Testing large array of ids " << endl;
    const int size = 1000;
    vector<Id> array(size);
    map<std::string, int> map;

    for (Id& id : array)
    {
        map[id.toData()] = 0;
    }

    
    bool ok = testValue(
        "Map has all with no duplicates",
        map.size() == size
    );

    BeeFishMisc::outputSuccess(ok);

    return ok;

}


}

#endif
