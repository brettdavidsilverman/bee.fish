#ifndef BEE_FISH_PARSER__BASE64_H
#define BEE_FISH_PARSER__BASE64_H

#include "../b-string/base64.h"

#include "parser.h"

namespace BeeFishParser
{
   class Base64 : public Match
   {
   
      const Or Base64Char = Or(
         new Range('0', '9'),
         new Range('a', 'z'),
         new Range('A', 'Z'),
         new Character('+'),
         new Character('/')
      );

    public:
       Base64() : Match()
       {
          _match = new And(
             new Repeat(Base64Char.copy()),
             new Optional(
                new Character('=')
             ),
             new Optional(
                new Character('=')
             )
          );
       }
    };
    
}


#endif