#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H

#include <stdio.h>
#include <ctype.h>
#include "word.h"

namespace BeeFishParser {

   using namespace std;

   class CIWord : public Word {
   public:
      CIWord(const BString& word) :
         Word(word.toLower())
      {
      
      }

   protected:
      virtual bool matchChar(const Char& character) {
         BString temp = character;
         return Word::matchChar(temp.toLower());
      }
   };

}

#endif
