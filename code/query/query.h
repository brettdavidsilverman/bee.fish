#ifndef BEE_FISH_QUERY_H
#define BEE_FISH_QUERY_H

#include <string>
#include <vector>
#include <iostream>
#include "../Miscellaneous/Debug.hpp"
#include <sstream>
#include <ostream>
#include <chrono>

#include "version.h"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../parser/parser.h"
#include "../Script/Variable.hpp"

using namespace BeeFishBString;
using namespace BeeFishScript;
using namespace BeeFishParser;

namespace BeeFishQuery {
    
    class Blankspace : public Or
    {
    public:
        Blankspace() : Or(
            new BeeFishParser::Character(" "),
            new BeeFishParser::Character("\t"),
            new BeeFishParser::Character("\r"),
            new BeeFishParser::Character("\n")
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
            new BeeFishParser::Or(
                new BeeFishParser::Character(letter),
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
    
    
    class Character : public BeeFishParser::Not
    {
    public:
        Character() : BeeFishParser::Not(
            new BeeFishParser::Or(
                new BeeFishParser::Character(" "),
                new BeeFishParser::Character("\t"),
                new BeeFishParser::Character("\r"),
                new BeeFishParser::Character("\n"),
                new BeeFishParser::Character("("),
                new BeeFishParser::Character(")"),
                new BeeFishParser::Character("+"),
                new BeeFishParser::Character("|"),
                new BeeFishParser::Character("-")

            )
        )
        {
        }
    };
    
    class Token : public BeeFishParser::Capture
    {
    public:
        Token() : BeeFishParser::Capture(
        
            new BeeFishParser::And(
                new Blankspaces(),
                new Repeat<BeeFishQuery::Character>(),
                new Blankspaces()
            )
        )
        {
        }
        
        virtual void fail()
        override
        {
            const BString& val = value().trim();
            if (val.length() &&
                val != "and" &&
                val != "or" &&
                val != "not")
            {
                Capture::success();
            }
            else
                Capture::fail();
        }
        
        virtual void success()
        override
        {
            const BString& val = value().trim();
            if (!val.length() ||
                val == "and" ||
                val == "or" ||
                val == "not")
            {
                Capture::fail();
            }
            else
                Capture::success();
        }
        
    };
    
    
    class Expression : public BeeFishParser::And
    {
    public:
        Expression() : BeeFishParser::And(
          //  new BeeFishParser::Character('('),
            new BeeFishParser::OrderOfPrecedence(

                // Not (expression)
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new LoadOnDemand<Expression>()
                ),
                
                // token1 and token2
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::And(),
                    new Token()
                ),
                /*
                // expression1 and expression2
                new BeeFishParser::And(
                    new LoadOnDemand<Expression>(),
                    new BeeFishQuery::And(),
                    new LoadOnDemand<Expression>()
                ),
                */
                // token1 or token2
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::Or(),
                    new Token()
                ),
            
                // (expression)
                new BeeFishParser::And(
                    new BeeFishParser::Character("("),
                    new LoadOnDemand<Expression>(),
                    new BeeFishParser::Character(")")
                ),
                        
            
                // token
                new Token()
                
            )
          //  new BeeFishParser::Character(")")
        )
        {
        }
        /*
        virtual void  eof(Parser* parser) override {
            
            if (result() != true)
                fail();
                
        }
        */
    };
    
}

#endif