#include "lexer.hpp"
#include "util/string_view.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>

using sk::Token;
using sk::TokenType;
using sk::Lexer;
using std::count_if;
using std::istringstream;

TEST(Token, constructs)
{
    EXPECT_NO_THROW(Token tok(TokenType::WHITESPACE, "   ", 4, 23));
}

TEST(Lexer, constructs)
{
    EXPECT_NO_THROW(Lexer lexer("foobar"));
}

TEST(Lexer, lexesEmptyStream)
{
    Lexer lexer("");
    EXPECT_EQ(TokenType::END_OF_INPUT, lexer.takeToken().getType());
}

TEST(Lexer, lexesNegativeNumber)
{
    Lexer lexer("-5");
    EXPECT_EQ(TokenType::MINUS, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::NUMBER, lexer.takeToken().getType());
}

TEST(Lexer, lexesPlusExpression)
{
    Lexer lexer("1 + 2");
    EXPECT_EQ(TokenType::NUMBER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    auto plusTok = lexer.takeToken();
    EXPECT_EQ(TokenType::PLUS, plusTok.getType());
    EXPECT_EQ("+", plusTok.getStr());
    EXPECT_EQ(3, plusTok.getCol());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
}

TEST(Lexer, lexesMultiLineFunction)
{
    Lexer lexer(
        R"(fn main() {
    2 * -8 + (6 / 2)
})" );
    lexer.takeToken();
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::IDENTIFIER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::OPEN_PAREN, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::CLOSE_PAREN, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::OPEN_BRACE, lexer.takeToken().getType());
}

TEST(Lexer, lexesFunctionWithParameters)
{
    Lexer lexer("fn foo(a, b, c) {}");
    EXPECT_EQ(TokenType::FN, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::IDENTIFIER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::OPEN_PAREN, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::IDENTIFIER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::COMMA, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::IDENTIFIER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::COMMA, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::IDENTIFIER, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::CLOSE_PAREN, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::WHITESPACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::OPEN_BRACE, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::CLOSE_BRACE, lexer.takeToken().getType());
}

TEST(Lexer, lexesComments)
{
    Lexer lexer("# here be dragons");
    EXPECT_EQ(TokenType::COMMENT, lexer.takeToken().getType());
    EXPECT_EQ(TokenType::END_OF_INPUT, lexer.takeToken().getType());
}
