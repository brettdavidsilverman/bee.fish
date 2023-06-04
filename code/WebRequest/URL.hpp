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
      
      typedef std::function<void (std::string)> Function;
      Function _onpath;

      Invoke::Function _oninvoke = nullptr;;

      Parser* _urlParser = nullptr;
   public:

      using Parser::read;

      URL(const URL& url) :
         _onpath(url._onpath)
      {
         _oninvoke =
            [this](Parser*) {
               if (_onpath) {
                  _onpath(_path);
               }
               _path.clear();
            };
         _urlParser = createParser();
      }

      URL(Function onpath) :
         _onpath(onpath)
      {

         _oninvoke =
            [this](Parser*) {
               if (_onpath)
                  _onpath(_path);
               _path.clear();
            };
         _urlParser = createParser();
      }

      virtual ~URL() {
         delete _urlParser;
      }

      Parser* createParser() {
         const auto seperator =
            Character("/") or
            Character("?");

         And parser =
         Repeat(
            seperator and
            Invoke(
               Capture(
                  Repeat(
                     not (
                        seperator or
                        blankSpace
                     ),
                     0
                  ),
                  _path
               ),
               _oninvoke
            ),
            1
         ) and Optional(seperator);

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
