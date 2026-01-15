#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <bitset>
#include "../b-string/b-string.h"
#include "../Miscellaneous/Miscellaneous.hpp"

using namespace std;

namespace BeeFishParser {

    class Parser;

    class Match {
    public:

        static const BString& EmptyString() {
            static const BString _EmptyString = {};
            return _EmptyString;
        }

        static unsigned long& matchInstanceCount() {
            static unsigned long _matchInstanceCount = 0;
            return _matchInstanceCount;
        }
        
        optional<bool> _result = nullopt;
        Match* _match = nullptr;
        Parser* _parser = nullptr;
        bool _delete = true;
        
    public:
    
        Match(void* params = nullptr)
        {
            ++matchInstanceCount();
        }

        Match(Match* match) :
            _match(match)
        {
            ++matchInstanceCount();
            if (_match)
                 _parser = _match->_parser;
        }
        
        virtual ~Match()
        {
            if (_match)
                delete _match;

            --matchInstanceCount();
        }
        
    public:
  
        virtual bool matched() const
        {
            return (result() == true);
        }
        

    public:

        virtual void setup(Parser* parser) {

            if (_parser)
                return;
                
            if (_match && !_match->_parser)
                _match->setup(parser);
                
            _parser = parser;

            if (_parser == nullptr)
                throw std::logic_error("Match::setup _parser is not defined.");

          
        }


        virtual bool match(
            Parser* parser,
            const Char& character
        )
        {
            if (!_parser)
                setup(parser);
                
            if (result() != nullopt)
                return false;
                
            bool matched = false;
            
            if (!_match) {
                matched = true;
            }
            else  {
                matched = _match->match(_parser, character);
            }
    
            if (result() == true)
                success();
            else if (result() == false) {
                fail();
            }
            
            return matched;
        }
        
        virtual bool match(Byte b)
        {
             return false;
        }
        
        virtual void success()
        {
//assert(_result == nullopt);
            setResult(true);
        }
        
        virtual void fail()
        {
//assert(_result == nullopt);
            setResult(false);
        }
        
        virtual Match* match()
        {
             if (_match)
                 return _match->match();
             return this;
        }
        
        virtual optional<bool> result() const
        {
             if (_result != nullopt)
                  return _result;
                  
             if (_match)
                  return _match->result();
                
             return nullopt;
        }
        
        virtual void setResult(optional<bool> newResult)
        {
            if (_result == nullopt)
                _result = newResult;
        }

        
        virtual const BString& value() const
        {
            if (_match)
                return _match->value();
                
            static BString emptyString = "";
            return emptyString;
        }
        
        virtual BString& value()
        {
            if (_match)
                return _match->value();
                
            static BString emptyString;
            emptyString = "";
            return emptyString;
        }
        
        virtual bool isOptional() const {
            if (_match)
                return _match->isOptional();
            return false;
        }
        
        virtual bool isLoadOnDemand() const {
            if (_match)
                return _match->isLoadOnDemand();
            return false;
        }
        
        // Defined in parser.h
        virtual const Char& character() const;

        virtual void eof(Parser* parser) 
        {

            if (!_parser)
                setup(parser);
                
            if (_match )
            {
                if (_match->result() == nullopt)
                    _match->eof(_parser);
                    
                if (_result == nullopt) {
                    if (_match->result() == true)
                         success();
                    else
                         fail();
                }
                
            }
            else if (_result == nullopt)
                fail();
          
        }
        
        
        virtual void write(ostream& out, Size tabIndex = 0) const {
            out << this->value();
        }
        
        friend ostream& operator << (ostream& out, const Match& match);
      
        friend istream& operator >> (istream& in, Match& number);
      
    
    };


}

#endif


