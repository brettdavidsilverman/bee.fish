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
#include "../b-string/b-string.h"

namespace BeeFishQuery {
    
    using namespace BeeFishBString;
    using namespace BeeFishParser;

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
    
    class Seperator : public BeeFishParser::Or
    {
    public:
        Seperator() : BeeFishParser::Or(
            new BeeFishParser::Character(" "),
            new BeeFishParser::Character("\t"),
            new BeeFishParser::Character("\r"),
            new BeeFishParser::Character("\n"),
            new BeeFishParser::Character("+"),
            new BeeFishParser::Character("|"),
            new BeeFishParser::Character("-"),
            new BeeFishParser::Character("("),
            new BeeFishParser::Character(")"),
            
            new BeeFishParser::And(
                
                new BeeFishParser::Or(
                    new BeeFishParser::CIWord(" and"),
                    new BeeFishParser::CIWord(" or"),
                    new BeeFishParser::CIWord(" not")
                ),
                
                new BeeFishParser::Or(
                    new BeeFishParser::Character(" "),
                    new BeeFishParser::Not(
                        new BeeFishParser::Character("(")
                    )
                )
            )
            
             
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
    
    class TokenCharacters : public Capture
    {
    public:
        TokenCharacters(): Capture(
            new Repeat<TokenCharacter>(1)
        )
        {
        }

    
    };
    
    class Token : public BeeFishParser::Match
    {
       
    public:
        Token() : BeeFishParser::Match()
        {
            _match = new BeeFishParser::And(
                new Blankspaces(),
                new Invoke(
                    new TokenCharacters(),
                    [](Match* match) {
                        cout << "*" << match->value() << "*" << endl;
                      //  assert(false);
                        return true;
                    }
                ),
                new Blankspaces()
            );
        }

        

        
    };
    
    
    template<typename T>
    class Bracketed : public BeeFishParser::And
    {
    public:
        Bracketed() : BeeFishParser::And(
            new Blankspaces(),
            new BeeFishParser::Character("("),
            new Blankspaces(),
            new LoadOnDemand<T>(),
            new Blankspaces(),
            new BeeFishParser::Character(")"),
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
                // not token
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Token()
                )
            },
            
            {
                // not expression
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new LoadOnDemand<Expression>()
                )
            },
            
            {
                // not (expression)
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Bracketed<Expression>()
                )
            },
            {
                // token and expression
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::And(),
                    new LoadOnDemand<Expression>()
                ),
            
                // token or expression
                new BeeFishParser::And(
                    new Token(),
                    new BeeFishQuery::Or(),
                    new LoadOnDemand<Expression>()
                )
                
            },
            {
                // (expression) and token
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::And(),
                    new Token()
                ),

                // (expression) or expression
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::Or(),
                    new Token()
                )
                
                
            },

            {
                // (expression) and expression
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::And(),
                    new LoadOnDemand<Expression>()
                ),

                // (expression) or expression
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::Or(),
                    new LoadOnDemand<Expression>()
                )
                
                
            },
            {
                // (expression) and (expression)
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::And(),
                    new Bracketed<Expression>()
                ),

                // (expression) or (expression)
                new BeeFishParser::And(
                    new Bracketed<Expression>(),
                    new BeeFishQuery::Or(),
                    new Bracketed<Expression>()
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
    
}

#endif