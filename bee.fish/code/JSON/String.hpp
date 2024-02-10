#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Parser.hpp"

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
      Invoke _parser;
      
      stringstream _output;
      
   public:
      using Parser::read;
      
      String() : _parser(createParser())
      {
      }
      
      String(const String& copy) :
          Parser(copy),
          _parser(createParser())
      {
      }
      
      
      
      Invoke createParser() {
         return
        
        
         Invoke(
         quote and
         Repeat(
            Invoke(
               Capture(
                  plainCharacter,
                  _plainCharacter
               ),
               [this](Parser* parser)
               {
                  _output << _plainCharacter;
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
                     _output << "\\";
                  else if (input == "\\\"")
                     _output << "\"";
                  else if (input == "\\r")
                     _output << "\r";
                  else if (input == "\\n")
                     _output << "\n";
                  else if (input == "\\b")
                     _output << "\b";
                  else if (input == "\\t")
                     _output << "\t";
                  else if (input == "\\f")
                     _output << "\f";
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
                        _output << c1;
                        
                     _output << c2;
                  }
                  else
                     _output << input;
                 
                  _escapedCharacter.clear();
                  return true;
               }
            ),
            0
         ) and
         quote,
         [this](Parser* parser)
         {
             _value = _output.str();
             _output.clear();
             return true;
         }
         );
      }
         
      Parser* copy() const override {
         return new String(*this);
      }

      virtual bool read(char c)
      override
      {
         return readIndirect(
            _parser,
            c
         );
      }
      
      
   };

   const String _string;
   
}

#endif
