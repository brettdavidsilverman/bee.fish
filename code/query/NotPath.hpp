#ifndef BEE_FISH__QUERY__NOT_PATH_HPP
#define BEE_FISH__QUERY__NOT_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class NotPath :
        public JoinPathBase<T>
    {
    protected:
        JoinPathBase<T>* _path;
        bool _ended;
    public:

        NotPath(JoinPathBase<T>* path) :
            _path(path)
        {
            _ended = _path->isDeadEnd();
        }
   
        virtual ~NotPath()
        {
            delete _path;
        }
        
        virtual bool canGoLeft() const
        override
        {
            return 
                not _path->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                not _path->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            if (!_ended &&
                _path->canGoLeft())
            {
                 _path->goLeft();
            }
            else
                _ended = true;
                
            
        }
        
        virtual void goRight()
        override
        {
            if (!_ended &&
                _path->canGoRight())
            {
                 _path->goRight();
            }
            else
                _ended = true;
        }
        
        virtual void goUp()
        override
        {
            assert(false);
            _path->goUp();
        }
        
        virtual bool isDeadEnd() {
            return (
                _path->canGoLeft() &&
                _path->canGoRight()
            );
        }

    
    };

}
#endif