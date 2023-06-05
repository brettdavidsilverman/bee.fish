#ifndef BEE_FISH__PARSER__INVOKE_HPP
#define BEE_FISH__PARSER__INVOKE_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <functional>
#include "ParserBase.hpp"

namespace BeeFishParser {

   using namespace std; 

   class Invoke : public Parser
   {
   public:
   
      typedef std::function<bool(Parser*)> Function;
      Function _function;
      Parser* _parser {nullptr};

   public:

      using Parser::read;
      using Parser::setResult;

      Invoke(const Parser& parser) :
         _parser(parser.copy())
      {

      }

      Invoke(const Invoke& source) :
         _parser(source._parser->copy()),
         _function(source._function)
      {

      }

      Invoke(
         const Parser& parser,
         Function func
      ) :
         _parser(parser.copy()),
         _function(func)
      {
      }

      virtual ~Invoke() {
         if (_parser)
            delete _parser;
      }

      virtual void setResult(
         std::optional<bool> result
      )
      override
      {
         if (result == true) {
            if (_function) {
               if (!_function(_parser)) {
                  setResult(false);
                  return;
               }
            }
         }

         Parser::setResult(result);

      }

      virtual bool read(char c)
      override
      {
         bool matched = _parser->read(c);
         if (_parser->_result != nullopt)
            setResult(_parser->_result);

         return matched;
      }

      virtual Parser* copy() const
      override
      {
         return new Invoke(*this);
      }
      
   
   };



}

#endif


