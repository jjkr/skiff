#pragma once

namespace sk
{
template <typename... Types>
class Visitor;

template <typename V>
class Visitor<V>
{
public:
    virtual void visit(V& visitable) = 0;
};

// specialization for multiple types
template <typename V, typename... Types>
class Visitor<V, Types...> : public Visitor<Types...>
{
public:
    // promote the function(s) from the base class
    using Visitor<Types...>::visit;

    virtual void visit(V& visitable) = 0;
};

template <typename Visitor>
class Visitable
{
public:
    virtual void accept(Visitor& visitor) = 0;
};
}