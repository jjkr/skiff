#include "lexer.hpp"
#include "util/string_view.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>

using sk::Token;
using sk::Lexer;
using std::count_if;
using std::istringstream;

TEST(Token, constructs)
{
    EXPECT_NO_THROW(Token tok(Token::Kind::WHITESPACE, "   ", 4, 23));
}

TEST(Lexer, constructs)
{
    EXPECT_NO_THROW(Lexer lexer("foobar"));
}

TEST(Lexer, lexesEmptyStream)
{
    Lexer lexer("");
    EXPECT_EQ(Token::Kind::END_OF_INPUT, lexer.takeToken().kind);
}

TEST(Lexer, lexesNegativeNumber)
{
    Lexer lexer("-5");
    EXPECT_EQ(Token::Kind::MINUS, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::NUMBER, lexer.takeToken().kind);
}

TEST(Lexer, lexesPlusExpression)
{
    Lexer lexer("1 + 2");
    EXPECT_EQ(Token::Kind::NUMBER, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::WHITESPACE, lexer.takeToken().kind);
    auto plusTok = lexer.takeToken();
    EXPECT_EQ(Token::Kind::PLUS, plusTok.kind);
    EXPECT_EQ("+", plusTok.str);
    EXPECT_EQ(3, plusTok.col);
    EXPECT_EQ(Token::Kind::WHITESPACE, lexer.takeToken().kind);
}

TEST(Lexer, lexesMultiLineFunction)
{
    Lexer lexer(
        R"(fn main() {
    2 * -8 + (6 / 2)
})" );
    lexer.takeToken();
    EXPECT_EQ(Token::Kind::WHITESPACE, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::IDENTIFIER, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::OPEN_PAREN, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::CLOSE_PAREN, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::WHITESPACE, lexer.takeToken().kind);
    EXPECT_EQ(Token::Kind::OPEN_BRACE, lexer.takeToken().kind);
}
