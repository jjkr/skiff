#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <memory>

namespace sk
{
class Parser
{
public:
    Parser(Lexer& lexer);

    std::unique_ptr<Module> parse();

private:
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parsePrimaryExpr();
    std::unique_ptr<Expr> parseParenExpression();
    std::unique_ptr<Expr> parseBinOpRhs(std::unique_ptr<Expr>&& lhs, int minPrecedence);
    std::unique_ptr<Expr> parseIdentifier();
    std::unique_ptr<Expr> parseNegativeNumber();
    std::unique_ptr<Expr> parseNumber();
    std::unique_ptr<Expr> parseFunctionDefinition();

    void consumeToken();

    Lexer& m_lexer;
    Token m_tok;
};
}
