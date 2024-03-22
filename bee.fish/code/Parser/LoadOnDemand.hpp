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

   public:
      Parser* _loadOnDemand = nullptr;
   public:
      typedef std::function<Parser*(void*)> Function;
   protected:
      const Function _function;
      void* _params = nullptr;
   public:

      using Parser::read;
      
      LoadOnDemand(Function function, void* params = nullptr) :
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
            return false;

      }

      virtual bool read(
         char character
      ) override
      {
 
         if (_result == false)
            return false;

         if (_loadOnDemand == nullptr) {
            _loadOnDemand = _function(_params);
            if (_loadOnDemand == nullptr) {
               setResult(false);
               return false;
            }
         }

         return readIndirect(
            *_loadOnDemand,
            character
         );
      }

      virtual Parser* copy()
      const override
      {
         return new LoadOnDemand(*this);
      }
      

   };

};

#endif
