#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      MatchPtr _first;
      MatchPtr _second;
      
   public:

      Or(MatchPtr first, MatchPtr second) :
         _first(first),
         _second(second)
      {
      }
      
      Or(const Or& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }
      
      virtual bool match(const Char& character)
      {
   
         bool matched = false;
         while (!matched && _result == nullopt)
         {
            if (_first->_result == nullopt)
            {
               matched |= _first->match(character);
               if (_first->_result == true)
               {
                  if (matched)
                     Match::match(character);
                  
                  success();
                  return matched;
               }
            }
         
            if (_second->_result == nullopt)
            {
               matched |= _second->match(character);
               if (_second->_result == true)
               {
                  if (matched)
                     Match::match(character);

                  success();
                  return matched;
               }
            }

            if ( ( _first->_result == false &&
                  _second->_result == false ) )
               fail();
         }
         
         if (matched)
            Match::match(character);
            
         return matched;
         
         
      }
   
   
      virtual Match& item() {
         if (_first->_result == true)
            return *_first;
         else if (_second->_result == true)
            return *_second;
         else
            throw runtime_error(
               "None of the items succeeded in Or"
            );
      }
      
      virtual size_t index()
      {
         if (_first->_result == true)
            return 0;
         else if (_second->_result == true)
            return 1;
         else
            throw runtime_error(
               "None of the items succeeded in Or"
            );
      }
      
			  virtual Match& operator[]
      (size_t index)
      {
         if (index == 0)
            return *_first;
         else if (index == 1)
            return *_second;

         throw runtime_error("Invalid index");
      }
      
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Or(*this));
      }
      
      virtual void reset()
      {
         Match::reset();
         _first->reset();
         _second->reset();
      }
      
      virtual void write(ostream& out) const
      {
         out << "Or";
         writeResult(out);
         out << "("
             << *_first
             << ", "
             << *_second
             << ")";
         
      }
      
      
   };


};

#endif
