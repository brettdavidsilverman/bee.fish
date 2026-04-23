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
    
        PathBase* _a;
        PathBase* _bounds;
        
    public:

        WordPath(
            const Path& words,
            const BString& word,
            PathBase* bounds
        )
        {
            
            BString lower = 
                word.toLower();
                
            if (words.contains(lower))
                _a = new Path(words[lower]);
            else
                _a = nullptr;
                
            _bounds = bounds;
        }
   
        virtual ~WordPath()
        {
            if (_a)
                delete _a;
            delete _bounds;
        }
        
        
        virtual bool canGoLeft() const
        override
        {
            return _a &&
                _a->canGoLeft() and
                _bounds->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return _a &&
                _a->canGoRight() and
                _bounds->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            _a->goLeft();
            _bounds->goLeft();
    
        }
        
        virtual void goRight()
        override
        {
            _a->goRight();
            _bounds->goRight();
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
            _bounds->goUp();
        }

        virtual void save()
        override
        {
            if (_a)
                _a->save();
            _bounds->save();
        }
        
        virtual void restore()
        override
        {
            if (_a)
                _a->restore();
            _bounds->restore();
        }
    
    };

}
#endif