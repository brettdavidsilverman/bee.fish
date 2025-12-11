#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace BeeFishJSON
{

template<class OpenBrace, class Item, class Seperator, class CloseBrace>
class Set : public Match
{
protected:
    Item* _setItem = nullptr;

protected:
    
    class _Seperator : public And {
    public:
        _Seperator() : And(
                new Optional(
                    new BlankSpace()
                ),
                new Seperator(),
                new Optional(
                    new BlankSpace()
                )
            )
        {

        }
    };

    class SubsequentItem : public And {
    protected:
        Set* _set;
        
    public:
        _Seperator* _seperator;
        Item* _item;
      
        SubsequentItem() : And() {
            throw std::runtime_error("SubsequentItem construted without a set");
        }

        SubsequentItem(Set* set) : And()
        {
            _set = set;
        }
        
        virtual void setup(Parser* parser)
        {
            if (_parser)
                return;
                
            _seperator = new _Seperator();
            _item = new Item(_set);
            
            _inputs = {
                _seperator,
                new Invoke(
                    _item,
                    [this](Match* match)
                    {
                        _set->onsetvalue(_item);
                        return true;
                    }
                )
            };
            
            And::setup(parser);
        }
        

    };

    typedef Repeat<SubsequentItem> _Repeat;
    
    class SubsequentItems : public _Repeat
    {
    protected:
        Set* _set;
    public:
        SubsequentItems() : _Repeat(0) {
            throw std::runtime_error("SubsequentItems constructed without set");
        }

        SubsequentItems(Set* set) : _Repeat(0)
        {
            _set = set;
        }

        virtual ~SubsequentItems() {
        }
        
        virtual SubsequentItem* createItem()
        override
        {
            SubsequentItem* item = new SubsequentItem(_set);
        
            if (_Repeat::_parser)
                item->setup(_Repeat::_parser);
            
            return item;
        }


    };


public:
    Set() : Match()
    {
    }

    virtual ~Set()
    {
    }

    virtual void setup(Parser* parser)
    {
        if (_parser)
            return;

        _match = new And(
            new Invoke(
                new OpenBrace(),
                [this](Match* match) {
                    this->onbeginset(match);
                    return true;
                }
            ),
            new Optional(
                new And(
                    new Invoke(
                        _setItem = createItem(),
                        [this](Match* match) {
                            this->onsetvalue(_setItem);
                            return true;
                        }
                    ),
                    new Optional(
                        new SubsequentItems(this)
                    )
                )
            ),
            new Invoke(
                new CloseBrace(),
                [this](Match* match) {
                    this->onendset(match);
                    return true;
                }
            )
        );
        
        Match::setup(parser);
        

    }
    
    virtual Item* createItem() 
    {
        Item* item =
            new Item(this);
                
        if (_parser)
            item->setup(_parser);
                
        return item;
    }
            
    virtual void onbeginset(Match* match)
    {
    }
    
    virtual void onsetvalue(Item* item)
    {
    }

    virtual void onendset(Match* match)
    {
    }


};


}

#endif

