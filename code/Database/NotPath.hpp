#ifndef BEE_FISH__DATABASE__NOT_PATH_HPP
#define BEE_FISH__DATABASE__NOT_PATH_HPP

#include "Iterable.hpp"


namespace BeeFishDatabase {

using namespace std;
using namespace BeeFishPowerEncoding;

    template<typename T>
    class NotPath :
        public Iterable<T>
    {
    protected:
        Iterable<T>* _path;
        bool _ended;
    public:

        NotPath(Iterable<T>* path ) :
            Iterable<T>(*path),
            _path(path)
        {
            _ended = _path->isDeadEnd();
        }
   
        virtual ~NotPath()
        {
            delete _path;
        }
        
        virtual bool canGoLeft(const Branch& branch) const
        override
        {
            return
                not _path->canGoLeft(branch);
        }
        
        virtual bool canGoRight(const Branch& branch) const
        override
        {
            return
                not _path->canGoRight(branch);
        }
        
        virtual void goLeft(const Branch& branch)
        override
        {
            if (!_ended &&
                _path->canGoLeft(branch))
            {
                 _path->goLeft(branch);
            }
            else
                _ended = true;
                
            
        }
        
        virtual void goRight(const Branch& branch)
        override
        {
            if (!_ended &&
                _path->canGoRight(branch))
            {
                 _path->goRight(branch);
            }
            else
                _ended = true;
        }

    
    };

}
#endif