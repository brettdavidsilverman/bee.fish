#ifndef BEE_FISH__PARSER__ARRAY_PARSER_H
#define BEE_FISH__PARSER__ARRAY_PARSER_H
#include <ostream>
#include <vector>
#include <memory>
#include "ParserBase.hpp"
#include "Optional.hpp"

using namespace std;

namespace BeeFishParser {

   class ArrayParser : public Parser {
   public:
      std::vector<Parser*> _inputs;
      size_t _index {0};
   public:

      ArrayParser(const ArrayParser& source) {
         for (auto parser : source._inputs) {
            _inputs.push_back(
               parser->copy()
            );
         }
      }

      ArrayParser(const ArrayParser& lhs, const ArrayParser& rhs)
      {

         for ( auto parser : lhs._inputs )
         {
            _inputs.push_back(
               parser->copy()
            );
         }

         for ( auto parser : rhs._inputs )
         {
            _inputs.push_back(
               parser->copy()
            );
         }


      }

      ArrayParser(const ArrayParser& lhs, const Parser& rhs)
      {

         for ( auto parser : lhs._inputs )
         {
            _inputs.push_back(
               parser->copy()
            );
         }

         _inputs.push_back(
            rhs.copy()
         );

      }

      ArrayParser(const Parser& lhs, const Parser& rhs)
      {
         _inputs.push_back(
            lhs.copy()
         );

         _inputs.push_back(
            rhs.copy()
         );

      }

      
      virtual ~ArrayParser()
      {
         for ( auto parser : _inputs )
         {
            delete parser;
         }
      }

      virtual size_t index() override {
         return _index;
      }

      virtual Parser*
      getByIndex(size_t index)
      override
      {
         return
            _inputs[index - 1];
      }

   };

};

#endif
