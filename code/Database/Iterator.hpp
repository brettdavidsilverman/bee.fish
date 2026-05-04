#ifndef BEE_FISH__DATABASE__ITERATOR_HPP
#define BEE_FISH__DATABASE__ITERATOR_HPP

namespace BeeFishDatabase {

template<typename T>
class Iterator {
public:
    PathBase* _path = nullptr; // The underlying pointer that the iterator wraps
    T _item;
    Stack _stack;
    bool _isEnd = true;

public:
    // Iterator traits (required for STL compatibility in C++17 and earlier)
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = const T&;

    // Constructor
    Iterator() {
    }


    Iterator(PathBase* path) :
        _path(path)
    {
        _isEnd = !_path->next<T>(_stack, _item);
    }


    // Dereference operator (*)
    reference operator*() const
    {
        return _item;
    }

    // Arrow operator (->)
    pointer operator->()
    {
        return &_item;
    }

    // Prefix increment operator (++)
    Iterator& operator++() {
        assert(!_isEnd);
        _isEnd = not
                 _path->next<T>(
                     _stack,
                     _item
                 );
        return *this;
    }

    // Postfix increment operator (++)
    Iterator operator++(int) {
        Iterator tmp = *this;
        tmp.save();
        ++(*this);
        tmp.restore();
        return tmp;
    }

    void save()
    {
        _path->save();
    }

    void restore()
    {
        _path->restore();
    }

    friend bool operator == (
        const Iterator& a,
        const Iterator& b
    )
    {
        return  (a._isEnd == b._isEnd);
    }

    friend bool operator != (
        const Iterator& a,
        const Iterator& b
    )
    {
        return (a._isEnd != b._isEnd);
    }


};
}

#endif