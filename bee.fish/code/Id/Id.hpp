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

      static String formatEpochMilliseconds(
         unsigned long milliseconds
      ) {
         std::stringstream stream;
         stream << milliseconds;
         return stream.str();
      }

      String epochMilliseconds() {
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
      
      
      String _key;
      String _name;
      Timestamp _timestamp;
      
      Id() : _timestamp()
      {
      }
      
      Id(
         const String& name
      ) :
         _name(name),
         _timestamp()
      {
      }

      Id(
         const String& name,
         unsigned long milliseconds,
         unsigned int increment
      ) :
         _name(name),
         _timestamp(milliseconds, increment)
      {
      }
      
      static Id fromKey(const String& key)
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
      
      const String& key()
      {
         if (_key.size() == 0)
            _key = createKey();
            
         return _key;
      }
      
      const Timestamp& timestamp() const
      {
         return _timestamp;
      }
      
      const String& name()
      {
         return _name;
      }
      
      String toString()
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
      
      String createKey()
      {
      
         BitStream stream;
         
         write(stream);

         if (stream._count != 0)
            throw logic_error("Id::createKey");

         // get the data
         Data data = stream.toData();

         return data.toBase64();
      }
      
      static Id decodeKey(const String& key) {
   
      
         // extract the raw data
         Data raw = Data::fromBase64(key);

         // extract the timestamp
         // from the key
         
         // Create a string of char bits
         BitStream stream(raw);
        
         // read the first "1"
         bool read =
            (stream.readBit() == 1);
         
         String name;
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
         
         if (!read || stream._count != 0)
            throw runtime_error("Invalid key");

         Id id(name, milliseconds, increment);
         id._key = key;
         
         return id;
      }
      

   };

}

#endif