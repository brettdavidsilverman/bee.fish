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
    protected:
        const BString _word;
        const BString _letter;
        
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
        ),
            _word(word),
            _letter(letter)
        {
        }
        
        friend ostream& operator <<
        (
            ostream& output,
            const Operator& _operator
        )
        {
            _operator.write(output);
            return output;
        }
        
        virtual void write(ostream& output, Size tabs = 0)
        const
        {
            output << _word;
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
    
    class WordCharacter : public BeeFishParser::Not
    {
    public:
        WordCharacter() :
            BeeFishParser::Not(
                new Seperator()
            )
        {
        }
    };
    
    class Word : public BeeFishParser::And
    {
    protected:
        BString _value;
        
    public:
        Word(): And(
            new Blankspaces(),
            new Capture(
                new Repeat<WordCharacter>(1),
                _value
            ),
            new Blankspaces()
        )
        {
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
    
    class AndOr : public BeeFishParser::Or
    {
    public:
        AndOr() : Or(
            new BeeFishQuery::And(),
            new BeeFishQuery::Or()
        )
        {
        }
        
        virtual void write(ostream& output, Size tabs = 0)
        const
        {
            output << (*_item);
        }
        
        friend ostream& operator <<
        (
            ostream& output,
            const AndOr& andOr
        )
        {
            andOr.write(output);
            return output;
        }
        
    };
    
    
    
    class Expression : public BeeFishParser::Match
    {
    public:
        class BracketedExpression : public BeeFishParser::And
        {
        protected:
            LoadOnDemand<Expression>* _item;
        
        public:
            BracketedExpression() : BeeFishParser::And(
                new Blankspaces(),
                new Character("("),
                new Blankspaces(),
                _item = new LoadOnDemand<Expression>(),
                new Blankspaces(),
                new Character(")"),
                new Blankspaces()
            )
            {
            }
        
            const Expression* item() const {
                return _item->item();
            }
            
            virtual void write(ostream& output, Size tabs = 0)
            const
            {
                output << "(" << *(item()) << ")";
            }
        
            friend ostream& operator <<
            (
                ostream& output,
                const BracketedExpression& bracketedExpression
            )
            {
                bracketedExpression.write(output);
                return output;
            }
            
        };
        
    protected:
        OrderOfPrecedence::Item* _notExpression = nullptr;
        OrderOfPrecedence::Item* _bracketedExpressionAndExpression = nullptr;
        OrderOfPrecedence::Item* _wordAndExpression = nullptr;
        OrderOfPrecedence::Item* _bracketedExpression = nullptr;
        OrderOfPrecedence::Item* _word = nullptr;
        
        LoadOnDemand<Expression>* _loadOnDemandExpression1 = nullptr;
        LoadOnDemand<Expression>* _loadOnDemandExpression2 = nullptr;
        LoadOnDemand<Expression>* _loadOnDemandExpression3 = nullptr;
        
        BracketedExpression* _bracketedExpression1 = nullptr;
        BracketedExpression* _bracketedExpression2 = nullptr;
        
        AndOr* _andOr1 = nullptr;
        AndOr* _andOr2 = nullptr;
        
        BeeFishQuery::Word* _word1 = nullptr;
        BeeFishQuery::Word* _word2 = nullptr;
        
    public:
        Expression() {
            _match = new BeeFishParser::And(
            new Blankspaces(),
            new BeeFishParser::OrderOfPrecedence(
                
        {
            {
                // not expression
                _notExpression = new OrderOfPrecedence::Item(
                    new BeeFishParser::And(
                        new BeeFishQuery::Not(),
                        _loadOnDemandExpression1 =
                            new LoadOnDemand<Expression>()
                    )
                )
            },
            {
                // (expression) and/or expression
               _bracketedExpressionAndExpression =
               new OrderOfPrecedence::Item(
                    new BeeFishParser::And(
                        _bracketedExpression1 =
                            new BracketedExpression(),
                        _andOr1 = new AndOr(),
                        _loadOnDemandExpression2 =
                            new LoadOnDemand<Expression>()
                    )
                )
            },
            {
                // word and/or expression
                _wordAndExpression =
                new OrderOfPrecedence::Item(
                    new BeeFishParser::And(
                        _word1 = new BeeFishQuery::Word(),
                        _andOr2 = new AndOr(),
                        _loadOnDemandExpression3 =
                            new LoadOnDemand<Expression>()
                    )
                )
            },
            {
                // (expression)
                _bracketedExpression =
                new OrderOfPrecedence::Item(
                    _bracketedExpression2 = new BracketedExpression()
                ),
            },
            {
                // word
                _word = new OrderOfPrecedence::Item(
                    _word2 = new BeeFishQuery::Word()
                )
            }
            
                 
        }
        
        )
        );
        
        }
        
        virtual void write(ostream& output, Size tabs = 0) const
        {
    
            if (_notExpression->matched())
            {
                output
                << "not "
                << *(_loadOnDemandExpression1->item());
            }
            else if (_bracketedExpressionAndExpression->matched())
            {
                    
                output 
                << *(_bracketedExpression1)
                << " "
                << *(_andOr1)
                << " "
                << *(_loadOnDemandExpression2->item());
                
            }
            else if (_wordAndExpression->matched())
            {
                output
                << *_word1
                << " "
                << *(_andOr2)
                << " "
                << *(_loadOnDemandExpression3->item());
            }
            else if (_bracketedExpression->matched())
            {
                output
                << *_bracketedExpression2;
            }
            else if (_word->matched())
                output << *_word2;
                
        }
        
        friend ostream& operator << (ostream& output, const Expression& expression)
        {
            expression.write(output);
            
            return output;
        }
    

    };


    class Statement : public BeeFishParser::Match
    {
    public:
        
        Capture* _capture;
        Expression* _expression;
    
    public:
        Statement() : BeeFishParser::Match()
        {
            _match = new BeeFishParser::And(
                _capture = new Capture(
                    _expression =
                        new Expression()
                ),
                new Blankspaces(),
                new Character(";")
            );
        }
        
        virtual BString& value() 
        override
        {
            return _capture->value();
        }
        
        virtual const BString& value() const
        override
        {
            return _capture->value();
        }
    };
    
    
}

#endif