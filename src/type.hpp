#pragma once
#include "lexer.hpp"
#include "util/string_view.hpp"

namespace sk
{
class Type
{
public:
    Type(Token definition);
    Type(string_view name);

    string_view getName() const { return m_name; }
    Token getDefinition() const { return m_definition; }

    bool isBuiltIn() const { return m_isBuiltIn; }

private:
    string_view m_name;
    Token m_definition;
    bool m_isBuiltIn;
};

}