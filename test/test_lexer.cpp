#include "lexer.hpp"
#include "util/string_view.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>

using sk::Token;
using sk::TokenKind;
using sk::Lexer;
using std::count_if;
using std::istringstream;

TEST(Token, constructs)
{
    EXPECT_NO_THROW(Token tok(TokenKind::WHITESPACE, "   ", 4, 23));
}

TEST(Lexer, constructs)
{
    EXPECT_NO_THROW(Lexer lexer("foobar"));
}

TEST(Lexer, lexesEmptyStream)
{
    Lexer lexer("");
    EXPECT_EQ(TokenKind::END_OF_INPUT, lexer.takeToken().getKind());
}

TEST(Lexer, lexesNegativeNumber)
{
    Lexer lexer("-5");
    EXPECT_EQ(TokenKind::MINUS, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::NUMBER, lexer.takeToken().getKind());
}

TEST(Lexer, lexesPlusExpression)
{
    Lexer lexer("1 + 2");
    EXPECT_EQ(TokenKind::NUMBER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    auto plusTok = lexer.takeToken();
    EXPECT_EQ(TokenKind::PLUS, plusTok.getKind());
    EXPECT_EQ("+", plusTok.getStr());
    EXPECT_EQ(3, plusTok.getCol());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
}

TEST(Lexer, lexesMultiLineFunction)
{
    Lexer lexer(
        R"(fn main() {
    2 * -8 + (6 / 2)
})" );
    lexer.takeToken();
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::OPEN_PAREN, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::CLOSE_PAREN, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::OPEN_BRACE, lexer.takeToken().getKind());
}

TEST(Lexer, lexesFunctionWithParameters)
{
    Lexer lexer("fn foo(a, b, c) {}");
    EXPECT_EQ(TokenKind::FN, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::OPEN_PAREN, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::COMMA, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::COMMA, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::CLOSE_PAREN, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::OPEN_BRACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::CLOSE_BRACE, lexer.takeToken().getKind());
}

TEST(Lexer, lexesComments)
{
    Lexer lexer("# here be dragons");
    EXPECT_EQ(TokenKind::COMMENT, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::END_OF_INPUT, lexer.takeToken().getKind());
}

TEST(Lexer, lexesLet)
{
    Lexer lexer("let x = 5");
    EXPECT_EQ(TokenKind::LET, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::IDENTIFIER, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::EQUALS, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::WHITESPACE, lexer.takeToken().getKind());
    EXPECT_EQ(TokenKind::NUMBER, lexer.takeToken().getKind());
}
