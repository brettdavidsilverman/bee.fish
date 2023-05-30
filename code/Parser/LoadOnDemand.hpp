#ifndef BEE_FISH__PARSER__LOAD_ON_DEMAND_H
#define BEE_FISH__PARSER__LOAD_ON_DEMAND_H
#include <ostream>
#include <vector>
#include <memory>
#include <functional>

#include "ParserBase.hpp"

using namespace std;

namespace BeeFishParser {

   template<typename T>
   class LoadOnDemand : public Parser {

   protected:
      Parser* _input = nullptr;
   public:
      typedef std::function<T()> Function;

   protected:
      const Function _function;

   public:

      LoadOnDemand(Function function) :
         _function(function)
      {
      }

      LoadOnDemand(const LoadOnDemand& source)
         : _function(source._function)
      {
      }

      virtual ~LoadOnDemand() {
         if (_input)
            delete _input;
      }

      virtual bool read(
         char character
      ) override
      {
         if (_input == nullptr) {
            _input = _function().copy();
         }

         bool matched = _input->read(
            character
         );

         if (_input->_result == true)
            setResult(true);

         if (_input->_result == false)
            setResult(false);

         return matched;
      }

      virtual Parser* copy()
      const override
      {
         return new LoadOnDemand(*this);
      }
      

   };

};

#endif
