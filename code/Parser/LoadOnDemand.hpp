#ifndef BEE_FISH__PARSER__LOAD_ON_DEMAND_H
#define BEE_FISH__PARSER__LOAD_ON_DEMAND_H
#include <ostream>
#include <vector>
#include <memory>
#include <functional>

#include "ParserBase.hpp"

using namespace std;

namespace BeeFishParser {

   class LoadOnDemand : public Parser {

   protected:
      Parser* _loadOnDemand = nullptr;
   public:
      typedef std::function<Parser*(Parser*)> Function;

   protected:
      const Function _function;
      Parser* _params = nullptr;
   public:

      LoadOnDemand(Function function, Parser* params = nullptr) :
         _function(function),
         _params(params)
      {
      }

      LoadOnDemand(const LoadOnDemand& source)
         : _function(source._function),
           _params(source._params)
      {
      }

      virtual ~LoadOnDemand() {
         if (_loadOnDemand)
            delete _loadOnDemand;
      }

      virtual bool isOptional() const {
         if (_loadOnDemand)
            return
               _loadOnDemand->isOptional();
         else
            return true;

      }

      virtual bool read(
         char character
      ) override
      {
 
         if (_loadOnDemand == nullptr) {
            _loadOnDemand = _function(_params);
         }

         bool matched = _loadOnDemand->read(
            character
         );

         if (_loadOnDemand->_result == true)
            setResult(true);

         if (_loadOnDemand->_result == false)
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
