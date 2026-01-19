#ifndef BEE_FISH_QUERY_HPP
#define BEE_FISH_QUERY_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <ostream>
#include <chrono>

#include "version.h"
#include "../parser/parser.h"
#include "../Database/Database.hpp"
#include "NotPath.hpp"
#include "AndPath.hpp"
#include "OrPath.hpp"

namespace BeeFishQuery {
    
using namespace BeeFishBString;
using namespace BeeFishParser;

    typedef Iterable<BString> Words;
    
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
        
        template<typename T>
        JoinPathBase<T>*
        getPath(
            Words& words
        )
        {
            return new Iterable<T>(
                words[value()]
            );
        }
        
    
    };
    
    class AndOr : public BeeFishParser::Or
    {
    public:
        BeeFishQuery::And* _and;
        BeeFishQuery::Or* _or;
        
    public:
        AndOr() : Or(
            _and = new BeeFishQuery::And(),
            _or = new BeeFishQuery::Or()
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
        
        template<typename T>
        JoinPathBase<T>*
        getPath(
            JoinPathBase<T>* a, 
            JoinPathBase<T>* b
        )
        {
            if (_and->matched())
                return new AndPath(a, b);
            else
                return new OrPath(a, b);
        }
        
    };
    

    
    class Expression : public BeeFishParser::Match
    {
    public:
        
        class LoadOnDemandExpression :
            public LoadOnDemand<Expression>
        {
                
        public:

            LoadOnDemandExpression()
            {
            }
            
        };
        
        class BracketedExpression : public BeeFishParser::And
        {
        protected:
            LoadOnDemand<Expression>* _item;
                
        public:
            BracketedExpression() : BeeFishParser::And(
                new Blankspaces(),
                new Character("("),
                new Blankspaces(),
                _item = new LoadOnDemandExpression(),
                new Blankspaces(),
                new Character(")"),
                new Blankspaces()
            )
            {
            }
        
            Expression* item() {
                return _item->item();
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
        
        LoadOnDemandExpression* _loadOnDemandExpression1 = nullptr;
        LoadOnDemandExpression* _loadOnDemandExpression2 = nullptr;
        LoadOnDemandExpression* _loadOnDemandExpression3 = nullptr;
        
        BracketedExpression* _bracketedExpression1 = nullptr;
        BracketedExpression* _bracketedExpression2 = nullptr;
        
        AndOr* _andOr1 = nullptr;
        AndOr* _andOr2 = nullptr;
        
        BeeFishQuery::Word* _word1 = nullptr;
        BeeFishQuery::Word* _word2 = nullptr;
        
    public:
        
        virtual bool onNotExpression(Expression* expression)
        {
            return true;
        }
        
        virtual bool onExpressionAndExpression(
            Expression* a,
            Expression* b
        )
        {
            return true;
        }
        
        virtual bool onExpressionOrExpression(
            Expression* a,
            Expression* b
        )
        {
            return true;
        }
        
        virtual bool onWordAndExpression(
            const BString& word,
            Expression* expression
        )
        {
            return true;
        }
        
        virtual bool onWordOrExpression(
            const BString& word,
            Expression* expression
        )
        {
            return true;
        }
        
        Expression()
        {
        }
        
        virtual void setup(Parser* parser)
        {
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
                            new LoadOnDemandExpression()
                    ),
                    [this](Match*)
                    {
                        if (not onNotExpression(
                                _loadOnDemandExpression1
                                ->item()
                            )
                        )
                        {
                            fail();
                            return false;
                        }
                        
                        return true;
                    }
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
                            new LoadOnDemandExpression()
                    ),
                    [this](Match*)
                    {
                        Expression* a = 
                            _bracketedExpression1->item();
                        Expression* b =
                            _loadOnDemandExpression2->item();
                                 
                        bool result;
                        
                        if (_andOr1->_and->matched())
                        {
                            
                            result =
                                onExpressionAndExpression(a, b);
                        }
                        else
                        {
                            result =
                                onExpressionOrExpression(a, b);
                        }
                        
                        if (!result)
                        {
                            fail();
                            return false;
                        }
                        
                        return true;
                                    
                                    
                    }
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
                            new LoadOnDemandExpression()
                    ),
                    [this](Match*)
                    {
                        BString& word =
                            _word->value();
                                        
                        Expression* expression =
                            _loadOnDemandExpression3->item();
                                
                        bool result;
                        if (_andOr2->_and->matched())
                        {
                            result =
                                onWordAndExpression(word, expression);
                        }
                        else
                        {
                            result =
                                onWordOrExpression(word, expression);
                        }
                        
                        if (!result)
                        {
                            fail();
                            return false;
                        }
                        
                        return true;
                                        
                    }
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
            Match::setup(parser);
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
        
        template<typename T>
        JoinPathBase<T>* getPath(Words words)
        {

            if (_notExpression->matched())
            {
                Expression* expression = 
                    _loadOnDemandExpression1
                    ->item();
                    
                JoinPathBase<T>* path = 
                    expression
                    ->getPath<T>(words);
                    
                return new NotPath<T>(path);
                
            }
            else if (_bracketedExpressionAndExpression->matched())
            {
                
                Expression* expressionA =
                    _bracketedExpression1->item();
                Expression* expressionB =
                    _loadOnDemandExpression2
                    ->item();
                    
                return
                    _andOr1->getPath<T>(
                        expressionA->getPath<T>(words),
                        expressionB->getPath<T>(words)
                    );
                    
                    /*
                output 
                << *(_bracketedExpression1)
                << " "
                << *(_andOr1)
                << " "
                << *(_loadOnDemandExpression2->item());
                */
                
            }
            else if (_wordAndExpression->matched())
            {
                JoinPathBase<T>* wordPath =
                    _word1->getPath<T>(words);
                    
                Expression* expression =
                    _loadOnDemandExpression3->item();
                    
                return
                    _andOr2->getPath<T>(
                        wordPath,
                        expression->getPath<T>(words)
                    );
                    /*
                output
                << *_word1
                << " "
                << *(_andOr2)
                << " "
                << *(_loadOnDemandExpression3->item());
                */
            }
            else if (_bracketedExpression->matched())
            {
                Expression* expression =
                    _bracketedExpression2->item();
                    
                return expression->getPath<T>(words);
                
                    /*
                output
                << *_bracketedExpression2;
                */
            }
            else if (_word->matched()) {
                return
                    _word2->getPath<T>(words);
                    /*
                output << *_word2;
                */
            }
            
            assert(false);
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
        
        template<typename T>
        Iterable<T>* getPath(Words words)
        {
            return _expression->getPath<T>(words);
        }
    };
    
    
}

#endif