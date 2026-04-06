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

typedef Path Words;
typedef Path Bounds;


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
    Blankspaces(Index count = 0) : Repeat(count)
    {
    }

};

class Operator : public BeeFishParser::Or
{
protected:
    const BString _word;
    const BString _letter;

public:
    Operator(
        const BString& word,
        const BString& letter
    ) : BeeFishParser::Or(
            new BeeFishParser::CIWord(word),
            new Character(letter)
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
    Not() : Operator("not", "!")
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
            new Character("!"),
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

class Word : public BeeFishParser::Capture
{
public:
    Word(): Capture(
            new Repeat<WordCharacter>(1)
        )
    {
    }

    virtual void success()
    override
    {
        if (value() == "and" ||
                value() == "or" ||
                value() == "not")
        {
            Capture::fail();
        }
        else
            Capture::success();
    }

    PathBase*
    getPath(
        Words& words,
        Path& bounds
    )
    {
        return new AndPath(
                   new Path(
                       words[value().toLower()]
                   ),
                   new Path(bounds),
                   new Path(bounds)
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

    PathBase*
    getPath(
        PathBase* a,
        PathBase* b,
        Path& bounds
    )
    {
        if (_and->matched())
            return new AndPath(a, b, new Path(bounds));
        else
            return new OrPath(a, b, new Path(bounds));
    }

};



class Expression : public BeeFishParser::Match
{

    class Item
    {
    public:
        enum class Type {
            And,
            Or,
            Not,
            Word,
            Expression
        } _type;

        Item(Type type) :
            _type(type)
        {
        }
        
        virtual ~Item()
        {
        }
        
        virtual void write(ostream& output) const
        {
            switch (_type)
            {
            case Type::And:
                output << "and";
                break;
            case Type::Or:
                output << "or";
                break;
            default:
                assert(false);
            }
        }
        
        friend ostream& operator << (
            ostream& output,
            const Item& item
        )
        {
            item.write(output);
            return output;
        }

    };

    class WordItem : public Item
    {
    public:
        BString _word;

        WordItem(BString& word) :
            Item(Type::Word),
            _word(word)
        {
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << _word;
        }
        
        
    };
    
    class NotItem : public Item
    {
    public:
        Expression* _expression;

        NotItem(Expression* expression) :
            Item(Type::Not),
            _expression(expression)
        {
            assert(_expression);
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << "not ";
            output << *_expression;
        }
    };
    
    class ExpressionItem : public Item
    {
    public:
        Expression* _expression;

        ExpressionItem(Expression* expression) :
            Item(Type::Expression),
            _expression(expression)
        {
            assert(_expression);
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << "(";
            output << *_expression;
            output << ")";
        }
    };


    class Token : public BeeFishParser::Or
    {
    protected:
        LoadOnDemand<Expression>* _loadOnDemand1;
        LoadOnDemand<Expression>* _loadOnDemand2;
    public:
        Token(Expression* expression) :
        Or(
            new Invoke(
                new BeeFishQuery::Word(),
                [expression](Match* match)
                {
                    expression->_stack.push_back(
                        new WordItem(match->value())
                    );
                    return true;
                }
            ),
            new Invoke(
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    _loadOnDemand1 =
                    new LoadOnDemand<Expression>()
                ),
                [expression, this](Match* match)
                {
                    expression->_stack.push_back(
                        new NotItem(_loadOnDemand1->item())
                    );
                    return true;
                }
            ),
            new Invoke(
                new BeeFishQuery::And(),
                [expression](Match* match)
                {
                    expression->_stack.push_back(
                        new Item(Item::Type::And)
                    );
                    return true;
                }
            ),
            new Invoke(
                new BeeFishQuery::Or(),
                [expression](Match* match)
                {
                    expression->_stack.push_back(
                        new Item(Item::Type::Or)
                    );
                    return true;
                }
            ),
            new Invoke(
                new BeeFishParser::And(
                    new Character('('),
                    _loadOnDemand2=
                    new LoadOnDemand<Expression>(),
                    new Character(')')
                ),
                [expression, this](Match* match)
                {
                    expression->_stack.push_back(
                        new ExpressionItem(_loadOnDemand2->item())
                    );
                    return true;
                }
            )
        )
        {
        }
    };


    class TokenAndBlankspace : public BeeFishParser::And
    {

    public:
        TokenAndBlankspace() {
            assert(false);
        }

        TokenAndBlankspace(Expression* expression) : And(
                new Token(expression),
                new Blankspaces()
            )
        {
        }
    };

    class Tokens : public Repeat<TokenAndBlankspace>
    {
    protected:
        Expression* _expression;
        vector<Match*> _delete;
    public:
        Tokens(Expression* expression) :
            Repeat(),
            _expression(expression)
        {
        }
        
        virtual ~Tokens()
        {
            for (auto match : _delete)
                delete match;
        }

        virtual TokenAndBlankspace* createItem() override {

            if (_item)
                delete _item;

            TokenAndBlankspace* item =
                new TokenAndBlankspace(_expression);

            if (_parser)
                item->setup(_parser);

            return item;
        }
        
        virtual void matchedItem(TokenAndBlankspace *match)
        {
            ++_matchedCount;

            _delete.push_back(_item);
            
            _item = nullptr;
        }

    };

public:
    vector<Item*> _stack;

    Expression() : Match(
            new BeeFishParser::And(
                new Blankspaces(),
                new Tokens(this)
            )
        )
    {
    }
    
    virtual ~Expression()
    {
        for (auto item : _stack)
            delete item;
    }
    
    virtual void write(
        ostream& output, 
        Size tabs = 0
    ) const
    {
        for (Index i = 0; i < _stack.size(); ++i)
        {
            const Item* item = _stack[i];
            output << *item;
            
            if (i < _stack.size() - 1)
                output << " ";
        }

    }

    friend ostream& operator << (ostream& output, const Expression& expression)
    {
        expression.write(output);

        return output;
    }

    PathBase* getPath(Words& words, Bounds& bounds)
    {
    return nullptr;
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
    /*
    PathBase* getPath(Words& words, Bounds& bounds)
    {
        return _expression->getPath(words, bounds);
    }
    */
};


}

#endif