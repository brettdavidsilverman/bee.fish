#ifndef BEE_FISH__QUERY__WORD_PATH_HPP
#define BEE_FISH__QUERY__WORD_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class WordPath :
        public PathBase
    {
    protected:
        Path _words;
        BString _word;
        PathBase* _a;
        
    public:

        WordPath(
            const Path& words,
            const BString& word
        ) :
            _words(words),
            _word(word)
        {
            
            BString lower = 
                word.toLower();
                
            if (words.contains(lower))
                _a = new Path(words[lower]);
            else
                _a = nullptr;
                
        }
        
        WordPath(const WordPath& source) :
            _words(source._words),
            _word(source._word)
        {
            if (source._a)
                _a = source._a->copy();
            else
                _a = nullptr;
        }
   
        virtual ~WordPath()
        {
            if (_a)
                delete _a;
        }
        
        virtual PathBase* copy() const
        override
        {
            return new WordPath(*this);
                
        }
        
        
        
        virtual bool canGoLeft() const
        override
        {
            return _a &&
                _a->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return _a &&
                _a->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            _a->goLeft();
    
        }
        
        virtual void goRight()
        override
        {
            _a->goRight();
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
        }

    };

}
#endif