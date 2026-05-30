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
#include "blankspaces.h"
#include "load-on-demand.h"
#include "order-of-precedence.h"


namespace BeeFishParser
{
using namespace BeeFishBString;
using namespace BeeFishScript;

class Match;

class Parser
{

protected:


    Size _charCount = 0;
    Size _offsetCharCount = 0;
    Size _dataBytes = Size(-1);
    String _error;
    optional<bool> _result = nullopt;
public:
    Match* _match;
    Match* _byteMatch = nullptr;
    Char _character = "";
    Char _lastCharacter;
    bool _deleteMatch = false;
    Index  _expectedBytes = 0;
    int _c = -1;
    int  _peek = -1;
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
        _dataBytes = Size(-1);

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
//             cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
        unsigned long start = now();
#endif

        if (!_match->_parser)
            _match->setup(this);

        char c;

        bool matched = true;
        
        
        while (
            matched &&
            (
                (_c = input.get()) != -1
            )
        )
        {
            _peek = input.peek();
            c = (char)_c;


            if (_dataBytes > 0 && _dataBytes != Size(-1))
            {

                --_dataBytes;

                // matched =
                _byteMatch->match((uint8_t)_c);

                if (_dataBytes == 0)
                {
                    _dataBytes = Size(-1);
                    success();
                    return true;
                }
            }
            else {

                if ((c &      0b11110000) == 0b11110000)
                    _expectedBytes = 4;
                else if ((c & 0b11100000) == 0b11100000)
                    _expectedBytes = 3;
                else if ((c & 0b11000000) == 0b11000000)
                    _expectedBytes = 2;
                else 
                    _expectedBytes = 1;
                    
                _lastCharacter = _character;
                _character.clear();
                
                while (1) 
                {

                    _character.push_back(c);
                    
                    if (--_expectedBytes > 0)
                    {
                        if ((_c = input.get()) == -1)
                            break;
                            
                        _peek = input.peek();
                        
                        c = (char)_c;
                        
                        if ((c & 0b10111111) == c)
                        {
                            ++_expectedBytes;
                        }
                    }
                    
                    if (_expectedBytes == 0)
                        break;
                }
                
                if (_c != -1) {
                    ++_charCount;
                    _lastCharacter = _character;
                    matched =
                        _match->match(this, _character);
                }
                
#if defined(DEBUG) && !defined(TIME)
    //cout << "{" << _character << "}";
#endif

            }

            if (_c == -1)
                break;

            if (_match->result() != nullopt)
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

        if (_match->result() == true)
            success();
        else if (_match->result() == false)
            fail();

        return result();
    }
    
    int peek() const
    {
        return _peek;
    }
    
    int c() const
    {
        return _c;
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
        return _result;
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

    void setDataBytes(Match* byteMatch, Size dataBytes) {
        _dataBytes = dataBytes;
        _byteMatch = byteMatch;
    }

    void markOffsetCharacterCount()
    {
        _offsetCharCount = _charCount;
    }

    virtual void eof() {

        if (result() != nullopt)
            return;

        if (_match)
            _match->eof(this);

        if (_match->result() == false)
        {
            fail();
        }
        else if (_match->result() == true)
        {
            success();
        }

    }

    virtual void success()
    {
        _result = true;
        _error.clear();
    }

    virtual void fail() {

        _result = false;
        setError();
    }

public:

    virtual void setError(BString error = "")
    {
        if (_error.length() == 0)
            _error = error;

        if (_error.length() == 0) {

            stringstream stream;

            stream << "Invalid Content '" << _lastCharacter << "' at position "
                   << (_charCount - _offsetCharCount + 1);

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



// Declared in match.h
const Char& Match::character() const {
    return _parser->_character;
}

ostream& operator << (ostream& out, const Match& match)
{
    match.write(out);
    return out;
}

istream& operator >> (istream& in, Match& match)
{
    Parser parser(match);
    parser.read(in);
    parser.eof();

    if (!parser.matched())
        throw runtime_error(parser.getError());

    return in;
}



}

#endif