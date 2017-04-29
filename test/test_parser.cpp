#include "parser.hpp"
#include "lexer.hpp"
#include <gtest/gtest.h>

using sk::Token;
using sk::Lexer;
using sk::Parser;

TEST(Parser, constructs)
{
    Lexer lexer("");
    Parser parser(lexer);
}

TEST(Parser, parsesEmpty)
{
    Lexer lexer("");
    Parser parser(lexer);
    auto ast = parser.parse();
}

TEST(Parser, parsesNumber)
{
    Lexer lexer("23");
    Parser parser(lexer);
    auto ast = parser.parse();
}

TEST(Parser, parsesPlusTimes)
{
    Lexer lexer("23 + 7 * 32");
    Parser parser(lexer);
    auto ast = parser.parse();
}

TEST(Parser, parsesFunction)
{
    Lexer lexer("fn foo(x, y) {}");
    Parser parser(lexer);
    auto ast = parser.parse();
}
