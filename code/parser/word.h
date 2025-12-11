#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include "../b-string/b-string.h"
#include "../Size.hpp"

namespace BeeFishParser {

    using namespace std;
                
    class Word : public Match {
    protected:
    
        BString _word;
        BString::const_iterator _index;
    
        virtual bool matchChar(const Char& character)
        {
            Size countMatches = 0;
            for (auto c : character)
            {
                if (c != (*_index))
                    return false;
                    
                countMatches++;
                
                if (++_index == _word.cend())
                    break;
            }
            
            return countMatches == character.size();
        }
    
    public:

        Word(BString word) :
            Match(),
            _word(word)
        {
            _index = _word.cbegin();
            _result = nullopt;
        }
      
        virtual bool match(Parser* parser, const Char& character)
        {
            setup(parser);
            
            if (result() != nullopt)
                return false;
            
            bool matched = matchChar(character);
            
            if (matched)
            {
                if (_index == _word.end())
                {
                    success();
                }
            
            }
            else
            {
                fail();
            }
         
            return matched;
        }
        
        virtual void eof(Parser* parser)
        {
            setup(parser);
            
            if (result() != nullopt)
                return;
                
            if (_index == _word.end())
                success();
            else
                fail();
        }
    
        const BString& word() const
        {
            return _word;
        }
    
    };


};

#endif
