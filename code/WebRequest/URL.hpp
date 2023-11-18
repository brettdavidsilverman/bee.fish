#ifndef BEE_FISH__WEB__URL_HPP
#define BEE_FISH__WEB__URL_HPP
#include "../Parser/ParserBase.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class URL : public Parser {
   protected:
      string _segment;
      
      typedef std::function<bool (std::string)> Function;
      Function _onsegment;

      Parser* _urlParser = nullptr;

      Invoke::Function  _oninvoke =
         [this](Parser*) {
            bool success = true;
            if (_onsegment) {
               success = _onsegment(_segment);
            }
            _segment.clear();
            return success;
         };
 
   public:

      using Parser::read;

      URL(const URL& url) :
         _onsegment(url._onsegment)
      {
        _urlParser = createParser();
      }

      URL(Function onsegment) :
         _onsegment(onsegment)
      {
         _urlParser = createParser();
      }

      virtual ~URL() {
         delete _urlParser;
      }

      Parser* createParser() {

         const auto seperator =
            Character("/") or
            Character("?") or
            Character(".");

         const And parser =
            Character("/") and
            Repeat(
               Invoke(
                  Capture(
                     Repeat(
                        not (
                           seperator
                        ),
                        1
                     ),
                     _segment
                  )
                  and seperator,
                  _oninvoke
               ),
               0
            );

         return parser.copy();

      }

      virtual bool read(char c)
      override
      {
         bool matched = _urlParser->read(c);

         if (_urlParser->_result != nullopt)
            setResult(_urlParser->_result);

         return matched;
      }

      Parser* copy() const override
      {
         return new URL(*this);
      }

   };

}

#endif
