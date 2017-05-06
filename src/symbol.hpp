#pragma once

namespace sk
{
class Symbol
{
public:
    Symbol(string_view name);

    enum Kind
    {
        TYPE,
        FUNCTION,
        VALUE
    };

    string_view getName() const { return m_name; }

    bool isType() const { return m_isType; };
    void setType(bool isType) { m_isType = isType; };
    bool isFunction() const { return m_isFunction; };
    void setIsFunction(bool isFunction) { m_isFunction = isFunction; };
    bool isValue() const { return m_isValue; };

private:
    string_view m_name;
    Kind m_kind;
};
}