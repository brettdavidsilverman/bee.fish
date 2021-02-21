#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include "match.h"
#include "bstring.h"

using namespace std;

namespace bee::fish::parser {
   
   class Capture : public Match
   {
   protected:
      Match* _item;
      bstring _value;

   public:
      Capture(
         const Match& item
      ) :
         _item(item.copy())
      {
      }
      
      Capture(const Capture& source) :
         _item(source._item->copy())
      {
      }
   
      virtual ~Capture()
      {
         delete _item;
      }
   
      virtual bool match(const Char& character)
      {
         bool matched =
            _item->match(character);
      
         if ( matched &&
              character != Match::EndOfFile )
             _value.push_back(character);

         if (_item->result() == true)
            success();
         else if (_item->result() == false)
            fail();
         
         
         return matched;
      }
   
      virtual bstring& value()
      {
         return _value;
      }
      
      virtual Match& item()
      {
         return *_item;
      }
   
      virtual Match* copy() const
      {
         return new Capture(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Capture";
         
         writeResult(out);
         
         out << "("
             << *_item
             << ")";
      }
   
   };



}

#endif


