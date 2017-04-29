#include "ast.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include <gtest/gtest.h>

using sk::Token;
using sk::Lexer;
using sk::Module;
using sk::Parser;

TEST(Parser, constructs)
{
    Lexer lexer("");
    Module module("");
    Parser parser(module, lexer);
}

TEST(Parser, parsesEmpty)
{
    Lexer lexer("");
    Module module("");
    Parser parser(module, lexer);
    parser.parse();
}

TEST(Parser, parsesNumber)
{
    Lexer lexer("23");
    Module module("");
    Parser parser(module, lexer);
    parser.parse();
}

TEST(Parser, parsesPlusTimes)
{
    Lexer lexer("23 + 7 * 32");
    Module module("");
    Parser parser(module, lexer);
    parser.parse();
}

TEST(Parser, parsesFunction)
{
    Lexer lexer("fn foo(x, y) {}");
    Module module("");
    Parser parser(module, lexer);
    parser.parse();
}
