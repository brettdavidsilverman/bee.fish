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
   protected:
      std::vector< std::shared_ptr<Parser> > _inputs;
      size_t _index {0};
   public:

      ArrayParser(const ArrayParser& source) {
         for (const std::shared_ptr<Parser>& parser : source._inputs) {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }
      }

      ArrayParser(const ArrayParser& lhs, const ArrayParser& rhs)
      {

         for ( auto parser : lhs._inputs )
         {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }

         for ( auto parser : rhs._inputs )
         {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }


      }

      ArrayParser(const ArrayParser& lhs, const Parser& rhs)
      {

         for ( auto parser : lhs._inputs )
         {
            _inputs.push_back(
               std::shared_ptr<Parser>(
                  parser->copy()
               )
            );
         }

         _inputs.push_back(
            std::shared_ptr<Parser>(
               rhs.copy()
            )
         );

      }

      ArrayParser(const Parser& lhs, const Parser& rhs)
      {
         _inputs.push_back(
            std::shared_ptr<Parser>(
               lhs.copy()
            )
         );

         _inputs.push_back(
            std::shared_ptr<Parser>(
               rhs.copy()
            )
         );

      }

      
      virtual ~ArrayParser()
      {
      }

      virtual size_t index() override {
         return _index;
      }

      virtual std::shared_ptr<Parser>
      operator [] (size_t index)
      override
      {
         return
            _inputs[index - 1];
      }

   };

};

#endif
