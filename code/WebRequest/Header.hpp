#ifndef BEE_FISH__WEB__HEADER_HPP
#define BEE_FISH__WEB__HEADER_HPP

#include <map>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <string>
#include <poll.h>

#include "../Miscellaneous/Miscellaneous.hpp"

#include "../Parser/Parser.hpp"
#include "../JSON/JSON.hpp"
#include "URL.hpp"

#include "Config.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class Headers;

   class Header : public And {
   public:
      string _key;
      string _value;

      
      And _Header()
      {
         const auto colon =
            Character(":");

         return
            Capture(
               Repeat(
                  not (
                     colon or
                     blankSpace)
               ),
               _key
            ) and
           -blanks and colon and
           -blanks and
            Capture(
               Repeat(
                  not (
                     carriageReturn or
                     lineFeed
                  )
               ),
               _value
            ) and
            newLine;
      };

   
   public:

      Header() : And(
        _Header()
      )
      {
      }

      Header(const Header& source) :
          And(_Header())
      {

      }

      virtual void success()
      override
      {

         // Convert key to lower case
         // Note that this only works
         // for latin characters
         std::transform(
            _key.begin(),
            _key.end(),
            _key.begin(),
            [](char c) {
               return std::tolower(c);
            }
         );

         std::transform(
            _value.begin(),
            _value.end(),
            _value.begin(),
            [](char c) {
               return std::tolower(c);
            }
         );

         And::success();

      }

      virtual Parser* copy() const
      override
      {
         return new Header(*this);
      }
   };

   class Headers :
      public Repeat,
      public map<string, string>
   {
   public:
      typedef std::function<void(Header*)> Function;
      Function _function;

      Headers(Function func = nullptr) :
         Repeat(Header()),
         _function(func)
      {
      }

      Headers(const Headers& source)
      :
         Repeat(Header()),
         _function(source._function)
      {
      }

      virtual void matchedItem(
         Parser* item
      )
      override
      {
         Header* header =
            dynamic_cast<Header*>(item);

         emplace(
            header->_key,
            header->_value
         );

         if (_function) {
            _function(header);
         }

         Repeat::matchedItem(item);
      }


      virtual Parser* copy() const
      override
      {
         return new Headers(*this);
      }
      
      const string& operator []  (
         const string& key
      ) const
      {
         static const string emptyString = "";

         if (count(key) > 0)
            return at(key);

         return emptyString;
      }

   };

   
}

#endif