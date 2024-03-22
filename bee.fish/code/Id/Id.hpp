#ifndef BEE_FISH_SERVER__ID_HPP
#define BEE_FISH_SERVER__ID_HPP

#include <string>
#include <iomanip>
#include <exception>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>

#include "../power-encoding/encoding.h"
#include "../b-string/string.h"
#include "../Script/Script.hpp"


using namespace std;
using namespace std::chrono;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;

namespace BeeFishId
{
   
   class Timestamp
   {
   public:
      unsigned long _nanoseconds;
      unsigned long _increment;

      Timestamp() :
         _nanoseconds(nanoseconds()),
         _increment(increment(_nanoseconds))
      {
      }
         
      Timestamp(
         unsigned long nanoseconds,
         unsigned long increment
      )
      {
         _nanoseconds = nanoseconds;
         _increment = increment;
      }
         
      static unsigned long nanoseconds()
      {
         unsigned long now =
            std::chrono::duration_cast
               <std::chrono::nanoseconds>
               (
                  std::chrono::system_clock
                     ::now()
                     .time_since_epoch()
               ).count();
         return now;
      }
      
      static std::string epoch() {
         unsigned long nanoseconds = Timestamp::nanoseconds();
         return Timestamp::formatEpochNanoseconds(
            nanoseconds
         );
      }

      static std::string formatEpochNanoseconds(
         unsigned long nanoseconds
      ) {
         std::stringstream stream;
         stream << nanoseconds;
         return stream.str();
      }

      std::string epochNanoseconds() {
         return Timestamp::formatEpochNanoseconds(
            _nanoseconds
         );
      }

   private:

      static unsigned long& lastNanoseconds() {
         static unsigned long _lastNanoseconds = 0;
         return _lastNanoseconds;
      }
      
      static unsigned long& lastIncrement() {
         static unsigned long lastIncrement = 0;
         return lastIncrement;
      }
         
      static unsigned long increment(
         unsigned long nanoseconds
      )
      {
         unsigned long& _lastNanoseconds = lastNanoseconds();
         unsigned long& _lastIncrement = lastIncrement();

         if (nanoseconds <= _lastNanoseconds)
            ++_lastIncrement;
         else
            _lastIncrement = 0;

         _lastNanoseconds = nanoseconds;
            
         return _lastIncrement;
      }
         
      
   };
      
   class Id
   {
      
   public:
      
      
      std::string _key;
      std::string _name;
      Timestamp _timestamp;
      
      Id() : _timestamp()
      {
      }
      
      Id(
         const std::string& name
      ) :
         _name(name),
         _timestamp()
      {
      }

      Id(
         const std::string& name,
         unsigned long nanoseconds,
         unsigned int increment
      ) :
         _name(name),
         _timestamp(nanoseconds, increment)
      {
      }
      
      static Id fromKey(const std::string& key)
      {
         return decodeKey(key);
      }
      
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {
      
         Id copy(id);
         
         out << copy.key();
         
         return out;
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
      
      const std::string& key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      const Timestamp& timestamp() const
      {
         return _timestamp;
      }
      
      const std::string& name()
      {
         return _name;
      }
      
      std::string toString()
      {

         BeeFishScript::Object output;
         
         output["name"] = _name;
         output["epoch ns"] =
            _timestamp.epochNanoseconds();
         output["increment"] = (BeeFishScript::Number)_timestamp._increment;

         stringstream stream;
         stream << output;
         return output.str();
      }
      
   private:
      
      PowerEncoding& write(
         PowerEncoding& stream
      ) const
      {
         // encode timestamp
         stream.writeBit(1);
         
         stream
            << _name
            << _timestamp._nanoseconds
            << _timestamp._increment;
         
         stream.writeBit(0);
         
         return stream;
      }
      
      std::string createKey()
      {
      
         BitStream stream;
         
         write(stream);

         assert(stream._count == 0);
         
         // get the data
         Data key = stream.toData();

         return key.toBase64();
      }
      
      static Id decodeKey(const std::string& key) {
   
      
         // extract the raw data
         Data raw = Data::fromBase64(key);

         // extract the timestamp
         // from the key
         
         // Create a string of char bits
         BitStream stream(raw);
        
         // read the first "1"
         bool read =
            (stream.readBit() == 1);
         
         std::string name;
         unsigned long nanoseconds;
         unsigned long increment;
         
         if (read) {
            
            stream >> name;
         
            // read milliseconds
            stream >> nanoseconds;
         
            // read increment
            stream >> increment;
         
            // read 0
            read = 
              (stream.readBit() == 0);
           
         }
         
         if (!read || stream._count != 0)
            throw runtime_error("Invalid key");

         Id id(name, nanoseconds, increment);
         id._key = key;
         
         return id;
      }
      

   };
   
   //unsigned long Timestamp::_lastMs = 0;
   //unsigned long Timestamp::_lastInc = 0;

 
}

#endif