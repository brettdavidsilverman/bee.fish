#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../Miscellaneous/Debug.hpp"
#include <sstream>
#include <ostream>
#include <chrono>

#include "version.h"
#include "../Size.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../b-string/b-string.h"
#include "../Script/Variable.hpp"

#include "match.h"
#include "capture.h"
#include "character.h"
#include "range.h"
#include "word.h"
#include "ciword.h"
#include "and.h"
#include "or.h"
#include "not.h"
#include "optional.h"
#include "repeat.h"
#include "invoke.h"
#include "blanks.h"
#include "load-on-demand.h"
#include "order-of-precedence.h"

using namespace BeeFishBString;
using namespace BeeFishScript;

namespace BeeFishParser
{
    
    class Match;
    
    class Parser
    {
        
    protected:

        
        Size _charCount = 0;
        SSize _dataBytes = -1;
        String _error;
        
    public:
        Match* _match;
        Match* _byteMatch = nullptr;
        Char _character = "";
        Char _lastCharacter = "";
        bool _deleteMatch = false;
        
    protected:
                 
        Parser() : _match(nullptr)
        {

        }
        
    public:
        
        Parser(Match* match) :
            _match(match)
        {

        }

        Parser(Match& match) :
            Parser(&match)
        {
        }
        
        
        void setMatch(Match& match, bool deleteMatch) {
            if (_match && _deleteMatch)
                delete _match;
            _match = &match;
            _charCount = 0;
            _dataBytes = -1;
            
            _deleteMatch = deleteMatch;
        }        

        virtual ~Parser()
        {
            if (_deleteMatch) {
                delete _match;
                _match = nullptr;
            }
        }
        
        Match* getMatch() {
            return _match;
        }

        unsigned long now()
        {
            return
                std::chrono::duration_cast
                <std::chrono::milliseconds>
                (
                    std::chrono::system_clock
                        ::now()
                            .time_since_epoch()
                ).count();
        }

        virtual optional<bool>
        read(
            istream& input
        )
        {
         
#ifdef TIME
            cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
            unsigned long start = now();
#endif

            if (!_match->_parser)
                _match->setup(this);
                
            int i = 0;
            uint8_t c;
            bool matched = true;
            while (
                    matched &&
                    (
                        (i = input.get()) != -1
                    )
                  )
            {
                
                c = (char)i;
                

                if (_dataBytes >= 0)
                {
    
                    --_dataBytes;
                    
                    matched = _byteMatch->match(c);
                    
                    if (_dataBytes == 0)
                    {
                        _dataBytes = -1;
                        success();
                        return true;
                    }
                }
                else {
                    while (c < 0) {
                    
                        _character.push_back(c);
                        
                        if ((i = input.get()) == -1)
                            break;
                    
                        c = (char)i;
                    
                    
                    }
                
                    _character.push_back(c);
                    ++_charCount;
#ifdef DEBUG
    cout << escape(_character);
#endif
                    _lastCharacter = _character;
                    
                    matched = _match->match(this, _character);
                        
                    _character.clear();
                
                }
                
                if (i == -1)
                    break;
                    
                if (result() != nullopt)
                    break;
                    
                    
            

                                    
#ifdef TIME
                if (_charCount % 100000 == 0)
                {
                    unsigned long time =
                        now() - start;
                    
                    cout 
                        << _charCount << " (char count)\t"
                        << Match::matchInstanceCount() << " (instances)\t" 
                        << time << " (milliseconds)\t"
                        << 100000.0 / (float)time << " (k chars per second)" 
                        << endl;

                    start = now();
                }
#endif

                
                    
            }
/*
            if (i == -1 && _match->result() == nullopt)
                _match->eof(this);

cerr << "PARSER I: " << -1 << endl;
cerr << "PARSER RESULT: " << result() << endl;
cerr << "MATCH RESULT: " << _match->result() << endl;
*/
            if (result() == true)
                success();
            else if (result() == false)
                fail();

            return result();
        }
    
        virtual optional<bool> read(const string& str)
        {
        
            istringstream input(str);
        
            return read(input);
        
        }
        
        virtual optional<bool> read(const BeeFishBString::BString& string)
        {

            istringstream input(string);
        
            return read(input);
        
        }
        
        virtual optional<bool> read(const char* string) {
            return read(BeeFishBString::BString(string));
        }
        
        virtual optional<bool> read(const char* string, Size length) {
            return read(BeeFishBString::BString(string, length));
        }

        
        
        virtual optional<bool> read(const char c)
        {

            BeeFishBString::BString bstring;
            bstring.push_back(c);
            return read(bstring);
        
        }
        
        optional<bool> result() const
        {
            if (_error.length())
                return false;

            return _match->result();
        }
        
        bool matched() const
        {
            return result() == true;
        }
        
        
        virtual bool isJSONParser() {
            return false;
        }

        virtual bool isBScriptParser() {
            return false;
        }

        void setDataBytes(Match* byteMatch, SSize dataBytes) {
            _dataBytes = dataBytes;
            _byteMatch = byteMatch;
        }
        
        virtual void eof() {

            if (result() == nullopt)
            {
            
                if (_match && 
                    _match->result() == nullopt)
                {
                    _match->eof(this);
                }
            
                if (result() == false)
                {
                    fail();
                }
                else if (result() == true)
                {
                    success();
                }
            }
            
        }

        virtual void success()
        {
            _match->setResult(true);
            _error.clear();
        }
        
        virtual void fail() {
             
            _match->setResult(false);
            setError();
        }
        
    public:
         
        virtual void setError(BString error = "")
        {
            if (_error.length() == 0)
                _error = error;
                
            if (_error.length() == 0) {
                
                stringstream stream;
                
                stream << "Invalid Content '" << escape(_lastCharacter) << "' at position "
                     << _charCount
                     << " from match "
                     << typeid(*match()).name();
                     
                _error = stream.str();
            
            }
            
        }
        
        virtual const BString& getError() const
        {
            return _error;
        }
        
        
        virtual Match* match()
        {
            if (_match)
                return _match->match();
                
            return nullptr;
        }
    };
    
    ostream& operator << (ostream& out, const Match& match)
    {
        match.write(out);
        return out;
    }
      
    istream& operator >> (istream& in, Match& match)
    {
        Parser parser(match);
        parser.read(in);
        return in;
    }
    
    // Declared in match.h
    const Char& Match::character() const {
        return _parser->_character;
    }
                
    // Declared in match.h
    void Match::fail()
    {
        setResult(false);
        
        if (_match)
            _match->fail();
        
        if (_parser && match() == _parser->match())
        {
            _parser->fail();
        }
    }
    /*
    void Match::fail(const BString& error)
    {
        _parser->setError(error);
        fail();
    }
    */
}

#endif