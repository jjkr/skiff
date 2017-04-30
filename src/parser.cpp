#include "parser.hpp"
#include "util/logger.hpp"
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::endl;
using std::make_unique;
using std::move;
using std::runtime_error;
using std::stoi;
using std::unique_ptr;
using std::vector;

namespace sk
{
Parser::Parser(Module& module, Lexer& lexer) : m_module(module), m_lexer(lexer),
                                               m_tok(TokenType::WHITESPACE, "", 0, 0)
{
}

void Parser::parse()
{
    m_tok = m_lexer.takeToken();
    consumeWhitespae();
    parseBlock(m_module.getMainBlock());
}

void Parser::parseBlock(Block& block)
{
    auto& expressions = block.getExpressions();
    auto hasBrace = m_tok.getType() == TokenType::OPEN_BRACE;
    if (hasBrace)
    {
        consumeToken();
    }
    auto expr = parseExpression();
    while (expr)
    {
        expressions.push_back(std::ref(*expr));
        block.addChild(move(expr));
        expr = parseExpression();
    }
    if (hasBrace)
    {
        if (m_tok.getType() != TokenType::CLOSE_BRACE) {
            loge << "Unexpected token " << m_tok << ", expected CLOSE_BRACE";
            throw runtime_error("Unexpected token");
        }
        consumeToken();
    }
}

unique_ptr<Expr> Parser::parseExpression()
{
    auto lhs = parsePrimaryExpr();
    if (!lhs)
    {
        return nullptr;
    }
    return parseBinOpRhs(move(lhs), 0);
}

unique_ptr<Expr> Parser::parsePrimaryExpr()
{
    switch (m_tok.getType())
    {
        case TokenType::IDENTIFIER:
            return parseIdentifier();
        case TokenType::NUMBER:
            return parseNumber();
        case TokenType::MINUS:
            return parseNegativeNumber();
        case TokenType::OPEN_PAREN:
            return parseParenExpression();
        case TokenType::FN:
            return parseFunctionDefinition();
        case TokenType::LET:
            return parseLetExpression();
        default:
            return nullptr;
    }
}

unique_ptr<Expr> Parser::parseBinOpRhs(unique_ptr<Expr>&& lhs, int minPrecedence)
{
    while (true)
    {
        auto precedence = getTokenPrecedence(m_tok.getType());
        if (precedence < minPrecedence)
        {
            return move(lhs);
        }

        auto opToken = m_tok;
        consumeToken();
        auto rhs = parsePrimaryExpr();

        auto nextPrecedence = getTokenPrecedence(m_tok.getType());
        if (precedence < nextPrecedence)
        {
            rhs = parseBinOpRhs(move(rhs), precedence + 1);
            if (!rhs)
            {
                return nullptr;
            }
        }

        lhs = unique_ptr<Expr>(new BinaryOp(opToken.getType(), move(lhs), move(rhs)));
    }
}

unique_ptr<Variable> Parser::parseIdentifier()
{
    auto expr = make_unique<Variable>(m_tok.getStr());
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseNegativeNumber()
{
    consumeToken();
    unique_ptr<Expr> expr(new I32Literal(-stoi(m_tok.getStr().to_string())));
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseNumber()
{
    unique_ptr<Expr> expr(new I32Literal(stoi(m_tok.getStr().to_string())));
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseFunctionDefinition()
{
    consumeToken(); // FN token
    if (m_tok.getType() != TokenType::IDENTIFIER)
    {
        loge << "Unexpected token " << m_tok << ", expected IDENTIFIER";
        throw runtime_error("Unexpected token");
    }
    auto name = m_tok.getStr();
    consumeToken();
    vector<string_view> parameters;
    if (m_tok.getType() == TokenType::OPEN_PAREN)
    {
        consumeToken(); // ( token
        if (m_tok.getType() != TokenType::CLOSE_PAREN)
        {
            while (true)
            {
                if (m_tok.getType() != TokenType::IDENTIFIER)
                {
                    loge << "Unexpected token " << m_tok << ", expected IDENTIFIER";
                    throw runtime_error("Unexpected token");
                }
                parameters.push_back(m_tok.getStr());
                consumeToken();
                if (m_tok.getType() == TokenType::CLOSE_PAREN)
                {
                    consumeToken();
                    break;
                }
                if (m_tok.getType() != TokenType::COMMA)
                {
                    loge << "Unexpected token " << m_tok << ", expected COMMA";
                    throw runtime_error("Unexpected token");
                }
                consumeToken();
            }
        }
    }
    auto func = std::make_unique<Function>(name, move(parameters));
    parseBlock(func->getBlock());
    return unique_ptr<Expr>(move(func));
}

unique_ptr<Expr> Parser::parseParenExpression()
{
    consumeToken(); // OPEN_PAREN token
    auto expr = parseExpression();
    if (m_tok.getType() != TokenType::CLOSE_PAREN)
    {
        throw runtime_error("Expected CLOSE_PAREN token");
    }
    consumeToken(); // CLOSE_PAREN token
    return expr;
}

unique_ptr<Expr> Parser::parseLetExpression()
{
    consumeToken(); // LET token
    auto id = parseIdentifier();
    if (m_tok.getType() != TokenType::EQUALS)
    {
        throw runtime_error("Expected EQUALS token");
    }
    consumeToken(); // EQUALS token
    auto expr = parseExpression();

    return unique_ptr<Expr>(new LetExpr(move(id), move(expr)));
}

void Parser::consumeWhitespae()
{
    while (m_tok.isWhitespace())
    {
        m_tok = m_lexer.takeToken();
    }
}

void Parser::consumeToken()
{
    consumeWhitespae();
    m_tok = m_lexer.takeToken();
    consumeWhitespae();
}
}
