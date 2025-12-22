#ifndef BEE_FISH_QUERY_H
#define BEE_FISH_QUERY_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <ostream>
#include <chrono>

#include "version.h"
#include "../parser/parser.h"

namespace BeeFishQuery {
    
    using namespace BeeFishBString;
    using namespace BeeFishParser;

    class Blankspace : public Or
    {
    public:
        Blankspace() : Or(
            new Character(" "),
            new Character("\t"),
            new Character("\r"),
            new Character("\n")
        )
        {
        }
    };
    
    class Blankspaces : public Repeat<Blankspace>
    {
    public:
        Blankspaces(size_t count = 0) : Repeat(count)
        {
        }
    };
    
    class Operator : public BeeFishParser::And
    {
    public:
        Operator(
            const BString& word,
            const BString& letter
        ) : BeeFishParser::And(
            new Blankspaces(),
            new BeeFishParser::Or(
                new Character(letter),
                new BeeFishParser::CIWord(word)
            ),
            new Blankspaces()
        )
        {
        }
        
    };
    
    class And : public Operator
    {
    public:
        And() : Operator("and", "+")
        {
        }
        
    };
    
    class Or : public Operator
    {
    public:
        Or() : Operator("or", "|")
        {
        }
        
    };
    
    class Not : public Operator
    {
    public:
        Not() : Operator("not", "-")
        {
        }
        
    };
    
    class Seperator : public BeeFishParser::Or
    {
    public:
        Seperator() : BeeFishParser::Or(
            new Character(" "),
            new Character("\t"),
            new Character("\r"),
            new Character("\n"),
            new Character("+"),
            new Character("|"),
            new Character("-"),
            new Character("("),
            new Character(")"),
            new Character(";")
        )
        {
        }
        
    };
    
    class TokenCharacter : public BeeFishParser::Not
    {
    public:
        TokenCharacter() :
            BeeFishParser::Not(
                new Seperator()
            )
        {
        }
    };
    
    class Token : public BeeFishParser::And
    {
    protected:
        BString _value;
        
    public:
        Token(): And(
            new Blankspaces(),
            new Capture(
                new Repeat<TokenCharacter>(1),
                _value
            ),
            new Blankspaces()
        )
        {
        }
        
        virtual void success() {
    cout << "*" << value() << "*";
            And::success();
        }
        
        const BString& value() const
        override
        {
            return _value;
        }
        
        BString& value()
        override
        {
            return _value;
        }
    
    };
    
    template<typename T>
    class Bracketed : public BeeFishParser::And
    {
    public:
        Bracketed() : BeeFishParser::And(
            new Blankspaces(),
            new Character("("),
            new Blankspaces(),
            new LoadOnDemand<T>(),
            new Blankspaces(),
            new Character(")"),
            new Blankspaces()
        )
        {
        }
    };
    
    class Expression : public BeeFishParser::And
    {
        
    public:
        Expression()
        : BeeFishParser::And(
            new Blankspaces(),
            new BeeFishParser::OrderOfPrecedence(
                
        {
            {
                // not expression
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new LoadOnDemand<Expression>()
                )
            },
            {
                // (expression) and expression
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishParser::Or(
                        new BeeFishQuery::And(),
                        new BeeFishQuery::Or()
                    ),
                    new LoadOnDemand<Expression>()
                )
            },
            {
                // token and expression
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishParser::Or(
                        new BeeFishQuery::And(),
                        new BeeFishQuery::Or()
                    ),
                    new LoadOnDemand<Expression>()
                )
            },
            {
                // (expression)
                new Bracketed<Expression>(),
            },
            {
                // token
                new Token()
            }
            
                 
        }
        
        )
        )
        {

        }
    

    };


    class Statement : public BeeFishParser::Match
    {
    public:
            
        Capture& expression = *(
            new Capture(
                new Expression()
            )
        );
        
    public:
        Statement() : BeeFishParser::Match()
        {
            _match = new BeeFishParser::And(
                &expression,
                new Blankspaces(),
                new Character(";")
            );
        }
    };
    
    
}

#endif