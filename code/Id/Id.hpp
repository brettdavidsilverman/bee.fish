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
#include "../b-string/b-string.h"
#include "../Script/Script.hpp"
#include "Version.hpp"

using namespace std;
using namespace std::chrono;
using namespace BeeFishPowerEncoding;
using namespace BeeFishBString;
using namespace BeeFishScript;

namespace BeeFishId
{
   
   class Timestamp
   {
   public:
      unsigned long _milliseconds;
      unsigned long _increment;

      Timestamp() :
         _milliseconds(milliseconds()),
         _increment(increment(_milliseconds))
      {
      }
         
      Timestamp(
         unsigned long milliseconds,
         unsigned long increment
      )
      {
         _milliseconds = milliseconds;
         _increment = increment;
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

   private:

      static unsigned long& lastMilliseconds() {
         static unsigned long _lastMilliseconds = 0;
         return _lastMilliseconds;
      }
      
      static unsigned long& lastIncrement() {
         static unsigned long lastIncrement = 0;
         return lastIncrement;
      }
         
      static unsigned long increment(
         unsigned long milliseconds
      )
      {
         unsigned long& _lastMilliseconds = lastMilliseconds();
         unsigned long& _lastIncrement = lastIncrement();

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
      /*
      friend ostream& operator <<
      (
         ostream& out, const Id& id
      )
      {
      
         Id copy(id);
         
         out << copy.key();
         
         return out;
      }
     */
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const Id& id
      )
      {
         
         id.write(stream);
         
         return stream;

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
      
      const BString& name()
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
      
   private:
      
      PowerEncoding& write(
         PowerEncoding& stream
      ) const
      {
         // encode timestamp
         stream.writeBit(1);

         stream
            << _name
            << _timestamp._milliseconds
            << _timestamp._increment;
         
         stream.writeBit(0);
         
         return stream;
      }
      
      BString createKey()
      {
      
         BitStream stream;
         
         write(stream);

         if (stream.count() != 0)
            throw logic_error("Id::createKey");

         // get the data
         std::string data = stream.toData();

         return toBase64(data);
      }
      
      static Id decodeKey(const BString& key) {
   
      
         // extract the raw data
         std::string raw =
            fromBase64(key);

         // extract the timestamp
         // from the key
         
         // Create a string of char bits
         BitStream stream(raw);
        
         // read the first "1"
         bool read =
            (stream.readBit() == 1);
         
         BString name;
         unsigned long milliseconds;
         unsigned long increment;
         
         if (read) {
            
            stream >> name;

            // read milliseconds
            stream >> milliseconds;
         
            // read increment
            stream >> increment;
         
            // read 0
            read = 
              (stream.readBit() == 0);
           
         }
         
         if (!read || stream.count() != 0)
            throw runtime_error("Invalid key");

         Id id(name, milliseconds, increment);
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