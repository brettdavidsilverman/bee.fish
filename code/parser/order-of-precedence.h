#ifndef BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H
#define BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H

#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"
#include "or.h"

namespace BeeFishParser {

    class OrderOfPrecedence : public Or {
    public:
        
        class Item : public Invoke {
        public:
            Item(Match* match) :
                Invoke(match)
            {
            }
            
            Item(Match* match, Function function) :
                Invoke(match, function)
            {
            }
            
            virtual void success()
            override
            {

                if (_function) {
                    if (!_function(_match))
                    {
                        fail();
                        return;
                    }
                }
            
                setResult(true);
                
            
            }
            
            virtual optional<bool> result() const
            override
            {
                return _result;
            }
        };
        
        typedef vector<vector<Item*> > Input;
        
    public:
        Input _input;
    public:

        OrderOfPrecedence(Input input) :
            Or(),
            _input(input)
        {
            for (vector<Item*>& order : input) {
                
                for (auto item : order) {
                    _inputs.push_back(item->_match);
                }
            }
            
            Or::_deleteInputs = false;

        }
        
        virtual ~OrderOfPrecedence()
        {
            for (Size index = 0;
                 index < _input.size();
                ++index)
            {
                vector<Item*>& items = _input[index];
                
                for (Item* item : items) {
                    delete item;
                }
            }
        }
        
        SSize findGroupIndex(Match* match)
        {
            for (SSize index = 0;
                 index < (SSize)_input.size();
                ++index)
            {
                vector<Item*>& items = _input[index];
                
                for (Item* item : items) {
                    if (item->_match == match)
                        return index;
                }
            }
            
            return -1;
            
        }
        
        virtual bool confirm(Match* match)
        override
        {
            SSize groupIndex = findGroupIndex(match);
            
            // Check that all higher
            // order of precedence
            // have no result so far
            for (SSize index = groupIndex - 1;
                index >= 0;
                --index)
            {
                vector<Item*>& items = _input[index];
                
                for (Item* higher : items) {
                    
                    if (higher->_match->result() != false)
                        return false;
                }
            }
            

            return true;
            
        }
        
        
        virtual void success()
        override
        {

            Or::success();
//cerr << "OrderOfPrecedence::success " << endl;

            for (Size index = 0;
                index < _input.size();
                ++index)
            {
                vector<Item*>& items = _input[index];
                
                for (Item* item : items) {
                    
                    if (item->_match == _item &&
                        item->result() == nullopt) {
//cerr << "Item::success " << endl;

                        item->success();
                        return;
                    }
                }
            }
            
            
        }
        
        

    };


};

#endif
