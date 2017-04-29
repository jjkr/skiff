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
Parser::Parser(Lexer& lexer) : m_lexer(lexer), m_tok(TokenType::WHITESPACE, "", 0, 0)
{
    consumeToken();
}

std::unique_ptr<Module> Parser::parse()
{
    m_module = make_unique<Module>("tempMod");
    parseBlock();
    //m_module->getMainBlock() = move(parseBlock());

    auto module = move(m_module);
    m_module = nullptr;
    return module;
}

std::unique_ptr<Block> Parser::parseBlock()
{
    std::unique_ptr<Block> block;
    //auto& expressions = block.getExpressions();
    auto expr = parseExpression();
    while (expr)
    {
        //expressions.push_back(move(expr));
        expr = parseExpression();
    }

    return block;
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
            logi << "parsing function";
            return parseFunctionDefinition();
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
    Variable name(m_tok.getStr());
    consumeToken();
    vector<unique_ptr<Variable>> parameters;
    if (m_tok.getType() == TokenType::OPEN_PAREN)
    {
        consumeToken(); // ( token
        while(m_tok.getType() != TokenType::CLOSE_PAREN)
        {
            if (m_tok.getType() == TokenType::COMMA) {
                consumeToken();
                continue;
            }
            parameters.push_back(parseIdentifier());
            //consumeToken();
        }
    }
    return unique_ptr<Expr>(new Function(move(name), move(parameters)));
}

unique_ptr<Expr> Parser::parseParenExpression()
{
    consumeToken(); // OPEN_PAREN token
    auto expr = parseExpression();
    if (m_tok.getType() != TokenType::CLOSE_PAREN) {
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
