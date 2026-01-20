#ifndef BEE_FISH_JSON__STRING_H
#define BEE_FISH_JSON__STRING_H

#include <iostream>
#include <fstream>

#include "../parser/parser.h"



namespace BeeFishJSON {

    using namespace BeeFishParser;
    
    class Quote : public BeeFishParser::Character {
    public:
        Quote() : Character('\"') {

        }
    };
    
    class CharacterBase : public Match
    {
    protected:
        Char _character;
        
    public:
        CharacterBase() : Match()
        {
        }
         
        virtual const Char& character() const
        override
        {
            return _character;
        }
        
    };

    class PlainCharacter : public CharacterBase
    {

    public:
        PlainCharacter() : CharacterBase() 
        {
            _match = new Not(
                new Or(
                    new BeeFishParser::
                        Character('\\'),
                    new BeeFishParser::
                        Character('\"'),
                    new BeeFishParser::
                        Character('\r'),
                    new BeeFishParser::
                        Character('\n'),
                    new BeeFishParser::
                        Character('\b'),
                    new BeeFishParser::
                        Character('\f')
                )
            );
        }
        
        virtual bool match(
             Parser* parser, 
             const Char& character
        )
        override
        {
            setup(parser);
            
            if (result() != nullopt)
                return false;
                
            bool matched = 
                _match->match(_parser, character);
                
            if (matched)
                _character = character;
                
            if (_match->result() == true)
                success();
            else if (_match->result() == false)
                fail();
                

            return matched;
        }
        
    };
    
    class HexCharacter : public Or {
    public:
        HexCharacter() : Or (
            new Range('0', '9'),
            new Range('a', 'f'),
            new Range('A', 'F')
        )
        {

        }
    };

    class Hex : public CharacterBase
    {
    private:
        Char _hexValue;

    public:
        Hex() : CharacterBase()
        {
             _match = new Capture(
                 new Repeat<HexCharacter>(
                     4, 4
                 )
             );
        }
                
        virtual void success()
        override
        {
            std::stringstream stream;
            stream << std::hex << value();
            uint32_t u32;
            stream >> u32;
            _hexValue = Char(u32);

            CharacterBase::success();

        }

        virtual const Char& character() const {
            return _hexValue;
        }
             
    };
        
    class EscapedCharacter :
        public CharacterBase
        
    {
    private:
        Hex* _hex = nullptr;
        
    protected:
        
        class CaptureCharacter :
            public Character
        {
        public:
            Char _input;
            Char _output;
            EscapedCharacter* _escapedCharacter;
            
        public:
            CaptureCharacter(
                EscapedCharacter* escapedCharacter,
                Char input,
                Char output
            ) : Character(input),
                _input(input),
                _output(output),
                _escapedCharacter(escapedCharacter)
            {
            
            }
            
            
            virtual void success() {
                _escapedCharacter->_character = _output;
                Character::success();
            }
        };
        
        
    public:
        EscapedCharacter() :
            CharacterBase()
        {
             
        }
        
        virtual void setup(Parser* parser)
        override
        {
            Match* invokeHex = new
                Invoke(
                    _hex = new Hex(),
                    [this](Match*)
                    {
                        _character = 
                            _hex->character();
                        return true;
                    }
                );
                
            _match = new And(
                new BeeFishParser::
                    Character('\\'),
                new Or(
                    new CaptureCharacter(this, '\\', '\\'),
                    new CaptureCharacter(this, 'b', '\b'),
                    new CaptureCharacter(this, 'f', '\f'),
                    new CaptureCharacter(this, 'r', '\r'),
                    new CaptureCharacter(this, 'n', '\n'),
                    new CaptureCharacter(this, 't', '\t'),
                    new CaptureCharacter(this, '\"', '\"'),
                    new And(
                        new PlainCharacter(),
                        new Not(new Hex())
                    ),
                    new And(
                        new BeeFishParser::
                            Character('u'),
                        invokeHex
                    )
                )
            );

            CharacterBase::setup(parser);
        }
        
        virtual ~EscapedCharacter() {
        }
        
        
    };
     
    class StringCharacter :
        public Match
    {
    protected:
    
        Or* _items = new Or(
            new PlainCharacter(),
            new EscapedCharacter()
        );
        
        
    public:
        StringCharacter()
        {
            _match = _items;
        }
        
        virtual const Char& character() const
        override
        {
            return _items->_item->character();
        }
        
    };
            
    class StringCharacters :
        public Repeat<StringCharacter>,
        public BeeFishBString::BStream,
        public BString
    {

    public:
        StringCharacters() : Repeat(0,0)
        {
        }

        virtual void matchedItem(StringCharacter* item)
        override
        {
            const Char& character = item->character();
            BString::push_back(character);
            BStream::write(character);
            Repeat::matchedItem(item);
        }
        
        virtual BString& value() {
            return *this;
        }
        
        virtual void clear() {
            BString::clear();
            BeeFishBString::BStream::clear();
        }

    };

    class String :
        public Match
    {
    public:
        BStream::OnBuffer _onbuffer;
        
    protected:
        StringCharacters*
            _stringCharacters = nullptr;
        BString _value;
        
    public:
        String() : Match()
        {
        }
        
        virtual void setup(Parser* parser)
        {
                
            _stringCharacters =
                new StringCharacters();
            
            _stringCharacters->_onbuffer =
                [this](const std::string& buffer) {
                    _value = _stringCharacters->value();
                    if (this->_onbuffer) {
                        this->_onbuffer(buffer);
                    }
                };


            _match = new And(
                new Quote(),
                _stringCharacters,
                new Quote()
            );


            Match::setup(parser);
        }
        
        virtual BString& value()
        {
            return _value;
        }
        
        virtual const BString& value() const
        {
            return _value;
        }
        
        virtual void success() {
            _stringCharacters->flush();
            Match::success();
        }
        
    protected:
        
    };


}

#endif