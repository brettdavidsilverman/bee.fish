#ifndef BEE_FISH__WEB__URL_HPP
#define BEE_FISH__WEB__URL_HPP
#include "../Parser/ParserBase.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class URL : public Parser {
   protected:
      string _path;
      
      typedef std::function<bool (std::string)> Function;
      Function _onpath;

      Parser* _urlParser = nullptr;

      Invoke::Function  _oninvoke =
         [this](Parser*) {
            bool success = true;
            if (_onpath) {
               success = _onpath(_path);
            }
            _path.clear();
            return success;
         };
 
   public:

      using Parser::read;

      URL(const URL& url) :
         _onpath(url._onpath)
      {
        _urlParser = createParser();
      }

      URL(Function onpath) :
         _onpath(onpath)
      {
         _urlParser = createParser();
      }

      virtual ~URL() {
         delete _urlParser;
      }

      Parser* createParser() {

         const auto seperator =
            Character("/") or
            Character("?");

         const auto parser =
         Repeat(
            Character("/") and
            Capture(
               Invoke(
                  Repeat(
                     not (
                        seperator or
                        blankSpace
                     ),
                     0
                  ),
                  _oninvoke
               ),
               _path
            ),
            1
         )
         and Optional(seperator);

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
