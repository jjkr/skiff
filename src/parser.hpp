#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <memory>

namespace sk
{
class Parser
{
public:
    Parser(Module& module, Lexer& lexer);

    void parse();

private:
    std::unique_ptr<Expr> parseExpression();
    void parseBlock(Block& block);
    std::unique_ptr<Expr> parsePrimaryExpr();
    std::unique_ptr<Expr> parseParenExpression();
    std::unique_ptr<Expr> parseBinOpRhs(std::unique_ptr<Expr>&& lhs, int minPrecedence);
    std::unique_ptr<Expr> parseIdExpression();
    std::unique_ptr<Identifier> parseIdentifier();
    std::unique_ptr<Expr> parseUnaryOperator();
    std::unique_ptr<Expr> parseNumber();
    std::unique_ptr<Expr> parseStringLiteral();
    std::unique_ptr<Expr> parseFunctionDefinition();
    std::unique_ptr<Expr> parseLetExpression();
    std::unique_ptr<Expr> parseIfExpression();
    std::unique_ptr<Match> parseMatch();
    std::unique_ptr<Match> parseIdMatch();
    std::unique_ptr<TupleMatch> parseTupleMatch();
    std::unique_ptr<TypeMatch> parseTypeMatch();

    void expectToken(TokenKind expected);

    void advance();
    Token takeToken();

    Module& m_module;
    Lexer& m_lexer;
    Token m_currentToken;
    Token m_nextToken;
};
}
