#include "ast.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "lexer.hpp"
#include <gtest/gtest.h>
#include <string>

using sk::Token;
using sk::Lexer;
using sk::I32Literal;
using sk::Module;
using sk::Parser;
using sk::SourceBuffer;
using sk::UnaryOp;
using std::to_string;
using namespace std::string_literals;

class ParserFixture : public ::testing::Test
{
public:
    ParserFixture() : lexer(buffer), module("parserTest"), parser(module, lexer) {}

protected:
    SourceBuffer buffer;
    Lexer lexer;
    Module module;
    Parser parser;
};

TEST(Parser, constructs)
{
}

TEST_F(ParserFixture, parsesEmpty)
{
    parser.parse();
}

TEST_F(ParserFixture, parsesInt)
{
    auto n = 23;
    buffer.addBlock(to_string(n));
    parser.parse();
    auto& expressions = module.getMainBlock().getExpressions();
    EXPECT_EQ(1, expressions.size());
    auto& intLiteral = dynamic_cast<I32Literal&>(expressions[0].get());
    EXPECT_EQ(n, intLiteral.getValue());
}

TEST_F(ParserFixture, parsesNegativeInt)
{
    auto n = -877;
    buffer.addBlock(to_string(n));
    parser.parse();
    auto& expressions = module.getMainBlock().getExpressions();
    EXPECT_EQ(1, expressions.size());
    auto& intLiteral = dynamic_cast<I32Literal&>(expressions[0].get());
    EXPECT_EQ(n, intLiteral.getValue());
}

TEST_F(ParserFixture, parsesUnaryOperator)
{
    buffer.addBlock("++32");
    parser.parse();
    auto& expressions = module.getMainBlock().getExpressions();
    EXPECT_EQ(1, expressions.size());
    auto& op = dynamic_cast<UnaryOp&>(expressions[0].get());
    EXPECT_EQ("++", op.getName());
}

TEST_F(ParserFixture, parsesPlusTimes)
{
    buffer.addBlock("23 + 7 * 32");
    parser.parse();
}

TEST_F(ParserFixture, parsesFunction)
{
    buffer.addBlock("fn foo(x, y) { 4 + 6 }");
    parser.parse();
}

TEST_F(ParserFixture, parsesUnaryFunction)
{
    buffer.addBlock("fn ident(x) { x }");
    parser.parse();
}

TEST_F(ParserFixture, parsesAnd)
{
    buffer.addBlock("0 and 1");
    parser.parse();
}

TEST_F(ParserFixture, parsesUnaryFunctionCall)
{
    buffer.addBlock("foo(5)");
    parser.parse();
}

TEST_F(ParserFixture, parsesStringLiteral)
{
    buffer.addBlock("\"foo\"");
    parser.parse();
}

TEST_F(ParserFixture, parsesIfStatement)
{
    buffer.addBlock("if 3 { 5 }");
    parser.parse();
}
