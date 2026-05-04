#ifndef BEE_FISH_SERVER__ID_HPP
#define BEE_FISH_SERVER__ID_HPP

#include <string>
#include <iomanip>
#include <exception>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <mutex>

#include "../power-encoding/encoding.h"
#include "../b-string/b-string.h"
#include "../Script/Script.hpp"
#include "Version.hpp"


namespace BeeFishId
{
using namespace boost::interprocess;
using namespace std::chrono;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;
using namespace BeeFishScript;

    struct Timestamp
    {
    public:
        unsigned long _milliseconds;
        unsigned long _increment;
        
        class SharedMemory {
        public:
            boost::interprocess::
                named_mutex _mutex;

            
            managed_shared_memory _segment;
            unsigned long* _lastMilliseconds;
            unsigned long* _lastIncrement;
                        
            SharedMemory() :
                _mutex(boost::interprocess::open_or_create, "BeeFishId::Timestamp::_mutex")
            {
#ifdef DEBUG
                
                //_mutex.unlock();
                //shared_memory_object::remove("BeeFishId::Timestamp");
                
#endif
                _segment = managed_shared_memory(open_or_create, "BeeFishId::Timestamp", 1000);
                _lastMilliseconds = 
                    _segment.find_or_construct<unsigned long>("lastMilliseconds")(milliseconds());
                    
                _lastIncrement = 
                    _segment.find_or_construct<unsigned long>("lastIncrement")(0);
            }
            
            ~SharedMemory()
            {
                _segment.destroy_ptr(_lastMilliseconds);
                _segment.destroy_ptr(_lastIncrement);
            }
            
        } inline static _memory;


        Timestamp()
        {
            std::scoped_lock lock(_memory._mutex);
                
            _milliseconds = milliseconds();
            _increment = increment(_milliseconds);

        }
        
        Timestamp(const Timestamp& source) :
            _milliseconds(source._milliseconds),
            _increment(source._increment)
        {
        }
            
        Timestamp(
            unsigned long milliseconds,
            unsigned long increment
        ) :
            _milliseconds(milliseconds),
            _increment(increment)
        {
            
        }
            
        static unsigned long milliseconds()
        {
            unsigned long now =
                std::chrono::duration_cast
                    <std::chrono::milliseconds>
                    (
                        std::chrono::system_clock
                            ::now()
                            .time_since_epoch()
                    ).count();
            return now;
        }
        
        static std::string epoch() {
            unsigned long milliseconds = Timestamp::milliseconds();
            return Timestamp::formatEpochMilliseconds(
                milliseconds
            );
        }

        static BString formatEpochMilliseconds(
            unsigned long milliseconds
        ) {
            std::stringstream stream;
            stream << milliseconds;
            return stream.str();
        }

        BString epochMilliseconds() const {
            return Timestamp::formatEpochMilliseconds(
                _milliseconds
            );
        }
        
        bool operator == (const Timestamp& timestamp)
        {
            return 
                _milliseconds == timestamp._milliseconds &&
                _increment == timestamp._increment;
        }
        
        bool operator != (const Timestamp& timestamp)
        {
            return 
                _milliseconds != timestamp._milliseconds or
                _increment != timestamp._increment;
        }
        
        friend PowerEncoding& operator <<
        ( 
            PowerEncoding& stream,
            const Timestamp& timestamp
        )
        {
            
            timestamp.write(stream);
            
            return stream;

        }
        
        
        void write(
            PowerEncoding& stream
        ) const
        {
            // encode timestamp
            stream.writeBit(1);

            stream
                << _milliseconds
                << _increment;
            
            stream.writeBit(0);
            
        }
        
        friend PowerEncoding& operator >>
        (PowerEncoding& stream, Timestamp& timestamp)
        {
            // read the first "1"
            bool read =
                (stream.readBit() == 1);

            if (read) {
                
                stream >> timestamp._milliseconds
                       >> timestamp._increment;
                
                // read 0
                read = 
                  (stream.readBit() == 0);
                  
            }
            
            if (!read)
                throw runtime_error("Invalid timestamp");

            return stream;
        }
        
        friend ostream& operator << (
            ostream& output,
            const Timestamp& timestamp
        )
        {
            output << "{\"milliseconds\": " 
                   << timestamp._milliseconds
                   << ", \"increment\": "
                   << timestamp._increment
                   << "}";
                   
            return output;
        }
        
        friend bool operator == (
            const Timestamp& a, 
            const Timestamp& b
        )
        {
            return a._milliseconds == b._milliseconds &&
                   a._increment == b._increment;
        }
        
        friend bool operator != (
            const Timestamp& a, 
            const Timestamp& b
        )
        {
            return a._milliseconds != b._milliseconds ||
                   a._increment != b._increment;
        }
        
        BString toString() const {
            // Convert to seconds
            std::time_t seconds = _milliseconds / 1000;
            auto milliseconds = _milliseconds % 1000;
            
            // Convert to local time structure
            std::tm* tm_ptr = std::localtime(&seconds);

            // Print human-readable time with milliseconds
            stringstream stream;
            stream << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S") 
                   << ":" << std::setfill('0') << std::setw(3) << milliseconds
                   << "." << std::setfill('0') << _increment;
            return stream.str();
        }
        
        BString toData() {
            Stack bits;
            bits << *this;
            assert(bits.count() == 0);
            return bits.toData();
        }
        
        static Timestamp fromData(const BString& data)
        {
            Stack bits(data);
            Timestamp timestamp;
            bits >> timestamp;
            return timestamp;
        }
        

    private:

        static unsigned long increment(
            unsigned long milliseconds
        )
        {
 
            static unsigned long& _lastMilliseconds
                = *_memory._lastMilliseconds;
            static unsigned long& _lastIncrement
                = *_memory._lastIncrement;

            if (milliseconds <= _lastMilliseconds)
                ++_lastIncrement;
            else
                _lastIncrement = 0;

            _lastMilliseconds = milliseconds;
            
            return _lastIncrement;
        }
            
        
    };
        
    class Id
    {
        
    public:
        
        
        BString _key;
        BString _name;
        Timestamp _timestamp;
        
        Id() : _timestamp()
        {
        }
        
        Id(
            const BString& name
        ) :
            _name(name),
            _timestamp()
        {
        }
        
        Id(
            const BString& name,
            const Timestamp& timestamp
        ) :
            _name(name),
            _timestamp(timestamp)
        {
        }

        Id(
            const BString& name,
            unsigned long milliseconds,
            unsigned int increment
        ) :
            _name(name),
            _timestamp(milliseconds, increment)
        {
        }
        
        static Id fromKey(const BString& key)
        {
            return decodeKey(key);
        }

        friend PowerEncoding& operator <<
        ( 
            PowerEncoding& stream,
            const Id& id
        )
        {
            
            id.write(stream);
            
            return stream;

        }
        
        friend PowerEncoding& operator >>
        (
            PowerEncoding& stream,
            Id& id
        )
        {
            bool read = (stream.readBit() == 1);
            if (read)
            {
                stream >> id._name
                       >> id._timestamp;
                       
                read = (stream.readBit() == 0);
            }
            
            if (!read)
                throw runtime_error("Invalid Id");
                
            return stream;
            
        }
        
        friend bool operator == (const Id& idA, const Id& idB)
        {
            return idA.timestamp() == idB.timestamp() &&
                   idA.name() == idB.name();
        }
        
        friend bool operator != (const Id& idA, const Id& idB)
        {
            return idA.timestamp() != idB.timestamp() ||
                   idA.name() != idB.name();
        }
        
        
        const BString& key()
        {
            if (_key.size() == 0)
                _key = createKey();
                
            return _key;
        }
        
        const Timestamp& timestamp() const
        {
            return _timestamp;
        }
        
        const BString& name() const
        {
            return _name;
        }
        
        std::string toString() const
        {

            BeeFishScript::Object output;
            
            output["name"] = _name;
            output["milliseconds"] =
                _timestamp.epochMilliseconds();
            output["increment"] = (BeeFishScript::Number)_timestamp._increment;

            stringstream stream;
            stream << output;
            return output.str();
        }
        
        BString toData() const {
            Stack bits;
            bits << *this;
            assert(bits.count() == 0);
            return bits.toData();
        }
        
        static Id fromData(const BString& data)
        {
            Stack bits(data);
            Id id;
            bits >> id;
            assert(bits.count() == 0);
            return id;
        }
        
    private:
        
        void write(
            PowerEncoding& stream
        ) const
        {
            // encode timestamp
            stream.writeBit(1);

            stream
                << _name
                << _timestamp;
                
            stream.writeBit(0);
            
        }
        
        BString createKey()
        {
        
            Stack stream;
            
            write(stream);

            if (stream.count() != 0)
                throw logic_error("Id::createKey");

            // get the data
            BString data = stream.toData();

            return toBase64(data);
            
        }
        
        static Id decodeKey(const BString& key) {
    
        
            // extract the raw data
            std::string raw =
                 fromBase64(key);

            // extract the timestamp
            // from the key
            
            // Create a string of char bits
            Stack stream(raw);
          
            // read the first "1"
            bool read =
                (stream.readBit() == 1);
            
            BString name;
            Timestamp timestamp;

            if (read) {
                
                stream >> name;

                // read timestamp
                stream >> timestamp;
                
                // read 0
                read = 
                  (stream.readBit() == 0);
              
            }
            
            if (!read)
                throw runtime_error("Invalid key");

            Id id(name, timestamp);
            id._key = key;
            
            return id;
        }
        
        
        

     };
    
     ostream& operator << (ostream& output, const Id& id)
     {
          output << id.toString();
          
          return output;
     }

}

#endif