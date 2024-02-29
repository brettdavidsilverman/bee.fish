#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Parser.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
      
   const auto quote = Character("\"");

   const auto plainCharacter =
      not (
         Character("\\") or
         Character("\"") or
         Character("\r") or
         Character("\n") or
         Character("\b") or
         Character("\t") or
         Character("\f")
      );

   const auto escapedCharacter =
      Character("\\") and (
      Character("\\") or
      Character("\"") or
      Character("r") or
      Character("n") or
      Character("b") or
      Character("t") or
      Character("f") or
      (
         Character("u") and
         Repeat(
            (
               Range("0", "9") or
               Range("A", "F") or
               Range("a", "f")
            ),
            4
         )
      )
   );

      
   class String :
       public Parser
   {
   protected:
      string _plainCharacter;
      string _escapedCharacter;
      string _value;
      Parser* _parser;
      
   public:
      using Parser::read;
      
      String() :
         Parser(),
         _value(""),
         _parser(createParser())
      {
      }
      
      String(const String& copy) :
          Parser(copy),
          _value(""),
          _parser(createParser())
      {
      }
      
      virtual ~String()
      {
         delete _parser;
      }
      
      
      Parser* createParser() {
         return (

         quote and
         Repeat(
            Invoke(
               Capture(
                  plainCharacter,
                  _plainCharacter
               ),
               [this](Parser* parser)
               {
                  _value += _plainCharacter;
                  _plainCharacter.clear();
                  return true;
               }
            ) or
            Invoke(
               Capture(
                  escapedCharacter,
                  _escapedCharacter
               ),
               [this](Parser* parser)
               {
                  string& input = _escapedCharacter;

                  bool result = true;
                  if (input == "\\\\")
                     _value += "\\";
                  else if (input == "\\\"")
                     _value += "\"";
                  else if (input == "\\r")
                     _value += "\r";
                  else if (input == "\\n")
                     _value += "\n";
                  else if (input == "\\b")
                     _value += "\b";
                  else if (input == "\\t")
                     _value += "\t";
                  else if (input == "\\f")
                     _value += "\f";
                  else if (input.size() == 6 &&
                     input[1] == 'u')
                  {
                     std::stringstream hex;
                     hex << std::hex << input.substr(2);
                     unsigned int word;
                     hex >> word;
                     char c1 = (0xFF00 & word) >> 8;
                     char c2 = (0x00FF & word);
                     
                     if (c1)
                        _value += c1;
                        
                     _value += c2;
                  }
                  else
                     _value += input;

                  _escapedCharacter.clear();
                  return true;
               }
            ),
            0
         ) and
         quote
         ).copy();
      }
      
      virtual bool read(char c)
      override
      {
         return readIndirect(
            *_parser,
            c
         );
      }
      
      virtual const
      std::string& value() const
      override
      {
         return _value;
      }
         
      Parser* copy() const override {
         return new String(*this);
      }
      
      
   };

   const String _string;
   
}

#endif
