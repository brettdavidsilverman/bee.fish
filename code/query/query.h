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
    
    class And : public BeeFishParser::Character
    {
    public:
        And() : BeeFishParser::Character("+")
        {
        }
        
    };
    
    class Or : public BeeFishParser:: Character 
    {
    public:
        Or() : BeeFishParser::Character("|")
        {
        }
        
    };
    
    class Not : public BeeFishParser:: Character
    {
    public:
        Not() : BeeFishParser::Character("-")
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
    
    
    class Token : public Repeat<BeeFishQuery::Character>
    {
    public:
        Token() : Repeat()
        {
        }
        
    };
            
    
    class AndWord : public BeeFishParser::And 
    {
    public:
        AndWord() : BeeFishParser::And(
            new Blankspaces(),
            new BeeFishQuery::And(),
            new Token()
        )
        {
        }
    };
    
    class AndWordList : public Repeat<AndWord>
    {
    public:
        AndWordList() : Repeat()
        {
        }
    
    };
    
    class Expression : public BeeFishParser::And
    {
    public:
        Expression() : BeeFishParser::And(
            new BeeFishParser::Character("("),
            new BeeFishParser::Or(
                // Word
                new BeeFishQuery::Token(),
                
                // Not operator
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Optional(new Blankspaces()),
                    new LoadOnDemand<Expression>()
                ),
                
                // And operator
                new Repeat<AndWord>(),
                
                // Or operator
                new BeeFishParser::And(
                    new LoadOnDemand<Expression>(),
                    new Optional(new Blankspaces()),
                    new BeeFishQuery::Or(),
                    new Optional(new Blankspaces()),
                    new LoadOnDemand<Expression>()
                ),
                
                // Sub expressions
                new BeeFishParser::And(
                    new BeeFishParser::Character("("),
                    new LoadOnDemand<Expression>(),
                    new BeeFishParser::Character(")")
                )
                        
                
            ),
            new BeeFishParser::Character(")")
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