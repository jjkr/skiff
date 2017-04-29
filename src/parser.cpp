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
Parser::Parser(Lexer& lexer) : m_lexer(lexer), m_tok(Token::Kind::WHITESPACE, "", 0, 0)
{
    consumeToken();
}

unique_ptr<Module> Parser::parse()
{
    vector<unique_ptr<Expr>> exprs;
    auto expr = parseExpression();
    while (expr)
    {
        exprs.push_back(move(expr));
        expr = parseExpression();
    }
    return make_unique<Module>(move(exprs));
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
    switch (m_tok.kind)
    {
        case Token::Kind::IDENTIFIER:
            return parseIdentifier();
        case Token::Kind::NUMBER:
            return parseNumber();
        case Token::Kind::MINUS:
            return parseNegativeNumber();
        case Token::Kind::OPEN_PAREN:
            return parseParenExpression();
        case Token::Kind::FN:
            return parseFunctionDefinition();
        default:
            return nullptr;
    }
}

unique_ptr<Expr> Parser::parseBinOpRhs(unique_ptr<Expr>&& lhs, int minPrecedence)
{
    while (true)
    {
        auto precedence = getTokenPrecedence(m_tok.kind);
        if (precedence < minPrecedence)
        {
            return move(lhs);
        }

        auto opToken = m_tok;
        consumeToken();
        auto rhs = parsePrimaryExpr();

        auto nextPrecedence = getTokenPrecedence(m_tok.kind);
        if (precedence < nextPrecedence)
        {
            rhs = parseBinOpRhs(move(rhs), precedence + 1);
            if (!rhs)
            {
                return nullptr;
            }
        }

        lhs = unique_ptr<Expr>(new BinaryOp(opToken.kind, move(lhs), move(rhs)));
    }
}

unique_ptr<Expr> Parser::parseIdentifier()
{
    unique_ptr<Expr> expr(new Variable(m_tok.str));
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseNegativeNumber()
{
    consumeToken();
    unique_ptr<Expr> expr(new I32Literal(-stoi(m_tok.str.to_string())));
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseNumber()
{
    unique_ptr<Expr> expr(new I32Literal(stoi(m_tok.str.to_string())));
    consumeToken();
    return expr;
}

unique_ptr<Expr> Parser::parseFunctionDefinition()
{
    consumeToken(); // FN token
    Variable name(m_tok.str);
    consumeToken(); // ( token
    vector<Variable> parameters;
    vector<unique_ptr<Expr>> expressions;
    unique_ptr<Expr> expr(new Function(move(name), move(parameters), move(expressions)));
    return expr;
}

unique_ptr<Expr> Parser::parseParenExpression()
{
    consumeToken(); // OPEN_PAREN token
    auto expr = parseExpression();
    if (m_tok.kind != Token::Kind::CLOSE_PAREN) {
        throw runtime_error("Expected CLOSE_PAREN token");
    }
    consumeToken(); // CLOSE_PAREN token
    return expr;
}

void Parser::consumeToken()
{
    do
    {
        m_tok = m_lexer.takeToken();
    } while (m_tok.isWhitespace());
}
}
