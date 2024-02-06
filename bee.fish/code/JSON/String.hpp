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
            Character("f")
         );

      const auto _string =
         quote and
         Repeat(
            plainCharacter or
            escapedCharacter,
            0) and
         quote;
         
   inline std::string unescape(
       const std::string& input
   )
   {
      string _plainCharacter;
      string _escapedCharacter;
      stringstream output;
      
      auto _string =
         quote and
         Repeat(
            Invoke(
               Capture(
                  plainCharacter,
                  _plainCharacter
               ),
               [&output, &_plainCharacter](Parser* parser)
               {
                  output << _plainCharacter;
                  _plainCharacter.clear();
                  return true;
               }
            ) or
            Invoke(
               Capture(
                  escapedCharacter,
                  _escapedCharacter
               ),
               [&output, &_escapedCharacter](Parser* parser)
               {
                  string c = _escapedCharacter;
                  bool result = true;
                  if (c == "\\\\")
                     output << "\\";
                  else if (c == "\\\"")
                     output << "\"";
                  else if (c == "\\r")
                     output << "\r";
                  else if (c == "\\n")
                     output << "\n";
                  else if (c == "\\b")
                     output << "\b";
                  else if (c == "\\t")
                     output << "\t";
                  else if (c == "\\f")
                     output << "\f";
                  else
                     result = false;
                     
                  _escapedCharacter.clear();
                  
                  return result;
               }
            ),
            0
         ) and
         quote;
         
      _string.read(input);
      
      return output.str();
   }
   

}

#endif
