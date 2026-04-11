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
#include "WordPath.hpp"

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
            Expression,
            Iterable
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

        WordItem(
            Expression& expression,
            BString& word
        ) :
            Item(Type::Word),
            _word(word)
        {
            IterableItem* iterable = new IterableItem(
                new BeeFishQuery::WordPath(
                    new Path(expression._words[_word]),
                    new Path(expression._bounds)
                ),
                _word
            );
            
            
            expression.push_back(iterable);
            
            delete this;
            
        }
        
        
    };
    
    class AndItem : public Item
    {
    public:

        AndItem(Expression& expression) :
            Item(Item::Type::And)
        {
            expression.push_back(this);
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << "and";
        }
    };
    
    class OrItem : public Item
    {
    public:

        OrItem(Expression& expression) :
            Item(Item::Type::Or)
        {
            expression.push_back(this);
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << "or";
        }
        
    };
    
    class NotItem : public Item
    {
    public:

        NotItem(
            Expression& expression
        ) :
            Item(Item::Type::Not)
        {
            Items& stack = expression._stack;
            
            if (stack.size() > 0)
            {
                Item* top = stack[stack.size() - 1];
                if (top->_type == Item::Type::Not)
                {
                    stack.pop_back();
                    delete top;
                    delete this;
                    return;
                }
                
            }
            
            expression.push_back(this);
        }
        /*
        NotItem(
            Expression& expression,
            Expression* notExpression) :
            Item(Item::Type::Not)
        {
            assert(notExpression->_stack.size() == 1);
            IterableItem* item =
                (IterableItem*)
                notExpression->_stack[
                     notExpression->_stack.size() - 1
                ];
                
            IterableItem* notItem =
                new IterableItem(
                    new NotPath(
                        item->_iterable,
                        new Path(
                            expression._bounds
                        )
                    ),
                    BString("not ") + item->_text
                );
                
            item->_iterable = nullptr;
            delete item;
            notExpression->_stack.pop_back();
            assert(notExpression->_stack.size() == 0);
        
            expression.push_back(notItem);
            
            delete this;
            
        }
        
        NotItem(
            Expression& expression,
            const BString& word
        ):
            Item(Item::Type::Not)
        {
            IterableItem* notItem =
                new IterableItem(
                    new NotPath(
                        new Path(
                            expression._words[word]
                        ),
                        new Path(
                            expression._bounds
                        )
                    ),
                    BString("not ") + word
                );
                
            expression.push_back(notItem);
            
            delete this;
            
        }
        */
    };
    
    class ExpressionItem : public Item
    {
    public:

        ExpressionItem(
            Expression& expression,
            Expression* bracketedExpression
        ) :
            Item(Item::Type::Expression)
        {
        
            assert(bracketedExpression->_stack.size() == 1);
            
            IterableItem* item =
                 (IterableItem*)
                 bracketedExpression->_stack[0];
            
            item->_text = 
                BString("(") +
                item->_text + 
                BString(")");
            
            bracketedExpression->_stack.pop_back();
            
            expression.push_back(item);
            
            delete this;
        }
        
    };
    
    class IterableItem : public Item
    {
    public:
        PathBase* _iterable = nullptr;
        
        BString _text;
        
        IterableItem(PathBase* iterable, const BString& text) :
            Item(Item::Type::Iterable),
            _iterable(iterable),
            _text(text)
        {
        }
        
        virtual ~IterableItem()
        {
            if (_iterable)
                 delete _iterable;
        }
        
        virtual void write(ostream& output) const
        override
        {
            output << _text;
        }
        
    };


    class LoadOnDemandExpression :
        public LoadOnDemand<Expression>
    {
    protected:
        Expression* _parent;
        
    public:
        LoadOnDemandExpression(Expression* parent) :
            LoadOnDemand<Expression>(),
            _parent(parent)
        {
        }
        
        virtual Expression* createItem() {
            return new Expression(
                *_parent->_database,
                _parent->_words,
                _parent->_bounds
            );
        }
    };
    
    class Token : public BeeFishParser::Or
    {
    protected:
        LoadOnDemandExpression* _loadOnDemand1;
        LoadOnDemandExpression* _loadOnDemand2;
        BeeFishQuery::Word* _word;
    public:
        Token(Expression* expression) :
        Or(
            new Invoke(
                new BeeFishQuery::Word(),
                [expression](Match* match)
                {
                    new WordItem(
                        *expression,
                        match->value()
                    );
                    
                    return true;
                }
            ),
            new Invoke(
                new BeeFishQuery::Not(),
                [expression, this](Match* match)
                {
                    new NotItem(
                        *expression
                    );
                    return true;
                }
            ),
            /*
            new Invoke(
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Blankspaces(),
                    _word = new BeeFishQuery::Word
                ),
                [expression, this](Match* match)
                {
                    new NotItem(
                        *expression,
                        _word->value()
                    );
                    return true;
                }
            ),
            new Invoke(
                new BeeFishParser::And(
                    new BeeFishQuery::Not(),
                    new Blankspaces(),
                    new BeeFishParser::Character("("),
                    _loadOnDemand1 =
                    new LoadOnDemandExpression(expression),
                    new BeeFishParser::Character(")")
                ),
                [expression, this](Match* match)
                {
                    new NotItem(
                        *expression,
                        _loadOnDemand1->item()
                    );
                    return true;
                }
            ),
            */
            new Invoke(
                new BeeFishQuery::And(),
                [expression](Match* match)
                {
                    new AndItem(*expression);
                    return true;
                }
            ),
            new Invoke(
                new BeeFishQuery::Or(),
                [expression](Match* match)
                {
                    new OrItem(*expression);
                    return true;
                }
            ),
            new Invoke(
                new BeeFishParser::And(
                    new Character('('),
                    _loadOnDemand2=
                    new LoadOnDemandExpression(expression),
                    new Character(')')
                ),
                [expression, this](Match* match)
                {
                    new ExpressionItem(
                        *expression,
                        _loadOnDemand2->item()
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
    
    class Items : public vector<Item*>
    {
    public:
        Items() : vector<Item*>() 
        {
        }
        
    } _stack;
    
    Database* _database;
    Path _words;
    Path _bounds;
    
    Expression()
    {
        assert(false);
    }
    
    Expression(Database& database, Path words, Path bounds) :
        Match(),
        _database(&database),
        _words(words),
        _bounds(bounds)
    {
    }
    
    Expression(JSONPath path) :
        Expression(
            path.database(),
            path.database().words(),
            path.database().objects()[path][OBJECT_CHILDREN]
        )
    {
    }
    
    Expression(JSONPath path, const BString& query) :
        Expression(path)
    {
        Parser parser(*this);
        parser.read(query);
        parser.eof();
        
        if (!parser.matched())
            throw runtime_error(parser.getError());
    }
        
    
    
    friend ostream& operator << (
        ostream& output,
        const Expression& expression
    )
    {
        expression.write(output);
        return output;
    }
    
    virtual void write(
        ostream& output
    ) const
    {
        for (const auto& item : _stack)
        {
            output << *item;
        }
    }
    
    virtual void success()
    override
    {
        if (_stack.size() != 1 ||
            _stack[0]->_type != Item::Type::Iterable)
            fail();
        else
            Match::success();
    }
    
    virtual ~Expression()
    {
        for (auto item : _stack)
            delete item;
    }
    
    virtual void setup(Parser* parser)
    {
        _match = new BeeFishParser::And(
            new Blankspaces(),
            new Tokens(this)
        );
        
        Match::setup(parser);
    }
    

    void push_back(Item* item)
    {
        if (_stack.size() >= 1 && 
            (item->_type == Item::Type::And ||
            item->_type == Item::Type::Or))
        {
            Item* top = _stack[_stack.size() - 1];
            if (top->_type == Item::Type::And ||
                top->_type == Item::Type::Or)
            {
                delete top;
                _stack.pop_back();
            }
         }
        _stack.push_back(item);
    }
    
    void push_back(IterableItem* right)
    {
        
        Item::Type type = Item::Type::And;
        IterableItem* left = nullptr;
            
        if (_stack.size() == 0)
        {
            _stack.push_back(right);

            return;
        }
        
        if (_stack.size() >= 1)
        {
            Item* top = _stack[_stack.size() - 1];
            if (top->_type == Item::Type::Not)
            {
                delete top;
                _stack.pop_back();
                
                IterableItem* item
                = new IterableItem(
                    new NotPath(
                        right->_iterable,
                        new Path(_bounds)
                    ),
                    BString("not ") +
                    right->_text
                );
                
                right->_iterable = nullptr;
                delete right;
                right = item;
                
                if (_stack.size() == 0)
                {
                    _stack.push_back(right);
                    return;
                }
                
            }
        }
        
        if (_stack.size() == 1)
        {
            left =
                (IterableItem*)
                _stack[_stack.size() - 1];
                
            type = Item::Type::And;
            _stack.pop_back();
        }
        else if (_stack.size() >= 2)
        {
            Item* op = _stack[_stack.size() - 1];
                
            if (op->_type == Item::Type::And ||
                op->_type == Item::Type::Or)
            {
                type = op->_type;
                _stack.pop_back();
                delete op;
                left =
                    (IterableItem*)
                        _stack
                        [_stack.size() - 1];
                _stack.pop_back();
            }
        }
                    
        Item* iterable;
        
        if (type == Item::Type::And)
        {
            BString text = BString("(") +
                 left->_text +
                 BString(" and ") +
                 right->_text +
                 BString(")");
                 
            iterable = new IterableItem(
                new AndPath(
                    left->_iterable,
                    right->_iterable,
                    new Path(_bounds)
                ),
                text
            );
            
            
        }
        else
        {
            BString text = "(" +
                 left->_text +
                 BString(" or ") +
                 right->_text +
                 BString(")");
                 
            iterable = new IterableItem(
                new OrPath(
                    left->_iterable,
                    right->_iterable,
                    new Path(_bounds)
                ),
                text
            );
        }
        
        left->_iterable = nullptr;
        right->_iterable = nullptr;
        
        delete left;
        delete right;
            
        _stack.push_back(iterable);
            
    }
    
    PathBase* getPath()
    {

        assert(_stack.size() == 1);
        assert(
            _stack[_stack.size() - 1]
            ->_type == Item::Type::Iterable
        );
        
        IterableItem* item =
            (IterableItem*)
            _stack[_stack.size() - 1];
            
        return item->_iterable;
        
    }
            
    

};


class Statement : public BeeFishParser::Match
{
public:

    Capture* _capture;
    Expression* _expression;

public:
    Statement(JSONPath path) :
        BeeFishParser::Match()
    {
        _match = new BeeFishParser::And(
            _capture = new Capture(
                _expression =
                    new Expression(path)
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