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
    
    class And : public BeeFishParser::Or
    {
    public:
        And() : BeeFishParser::Or(
            new BeeFishParser::Character("+"),
            new BeeFishParser::CIWord("and ")
        )
        {
        }
        
    };
    
    class Or : public BeeFishParser::Or
    {
    public:
        Or() : BeeFishParser::Or(
            new BeeFishParser::Character("|"),
            new BeeFishParser::CIWord("or ")
        )
        {
        }
        
    };
    
    class Not : public BeeFishParser::Or
    {
    public:
        Not() : BeeFishParser::Or(
            new BeeFishParser::Character("-"),
            new BeeFishParser::CIWord("not ")
        )
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
                new BeeFishQuery::And(),
                new BeeFishQuery::Or(),
                new BeeFishQuery::Not()
            )
        )
        {
        }
    };
    
    class Token : public BeeFishParser::And
    {
    public:
        Token() : BeeFishParser::And(
            new Blankspaces(),
            new Repeat<BeeFishQuery::Character>(),
            new Blankspaces()
        )
        {
        }
        
    };
    
    /*
    class Token : public Repeat<BeeFishQuery::Character>
    {
    public:
        Token() : Repeat()
        {
        }
        
    };
            
    */
    class AndWord : public BeeFishParser::And 
    {
    public:
        AndWord() : BeeFishParser::And(
            new Token(),
            new BeeFishQuery::And(),
            new Token()
        )
        {
        }
    };
    
    
    class Expression : public BeeFishParser::And
    {
    public:
        Expression() : BeeFishParser::And(
           // new BeeFishParser::Character("("),
            new BeeFishParser::Or(
                // Word
                //new BeeFishQuery::And(),
                
                // Not operator
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Token()
                ),
                
                // And operator
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::And(),
                    new Token()
                ),
                
                // Or operator
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::Or(),
                    new Token()
                ),
                
                // Sub expressions
                new BeeFishParser::And(
                    new BeeFishParser::Character("("),
                    new LoadOnDemand<Expression>(),
                    new BeeFishParser::Character(")")
                )
                        
                
            )
           // new BeeFishParser::Character(")")
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