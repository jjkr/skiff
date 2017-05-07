#include "parser.hpp"
#include "util/logger.hpp"
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::endl;
using std::make_unique;
using std::move;
using std::ostringstream;
using std::runtime_error;
using std::stoi;
using std::swap;
using std::unique_ptr;
using std::vector;

namespace sk
{
Parser::Parser(Module& module, Lexer& lexer) : m_module(module), m_lexer(lexer)
{
}

void Parser::parse()
{
    m_currentToken = takeToken();
    m_nextToken = takeToken();
    parseBlock(m_module.getMainBlock());
}

void Parser::parseBlock(Block& block)
{
    auto& expressions = block.getExpressions();
    auto hasBrace = m_currentToken.getKind() == TokenKind::OPEN_BRACE;
    if (hasBrace)
    {
        advance();
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
        if (m_currentToken.getKind() != TokenKind::CLOSE_BRACE) {
            loge << "Unexpected token " << m_currentToken << ", expected CLOSE_BRACE";
            throw runtime_error("Unexpected token");
        }
        advance();
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
    switch (m_currentToken.getKind())
    {
        case TokenKind::IDENTIFIER:
            return parseIdExpression();
        case TokenKind::NUMBER:
            return parseNumber();
        case TokenKind::STRING_LITERAL:
            return parseStringLiteral();
        case TokenKind::OPERATOR:
            return parseUnaryOperator();
        case TokenKind::OPEN_PAREN:
            return parseParenExpression();
        case TokenKind::FN:
            return parseFunctionDefinition();
        case TokenKind::LET:
            return parseLetExpression();
        case TokenKind::IF:
            return parseIfExpression();
        default:
            return nullptr;
    }
}

unique_ptr<Expr> Parser::parseBinOpRhs(unique_ptr<Expr>&& lhs, int minPrecedence)
{
    while (true)
    {
        auto precedence = getTokenPrecedence(m_currentToken);
        if (precedence < minPrecedence)
        {
            return move(lhs);
        }

        auto opToken = m_currentToken;
        advance();
        auto rhs = parsePrimaryExpr();

        auto nextPrecedence = getTokenPrecedence(m_currentToken);
        if (precedence < nextPrecedence)
        {
            rhs = parseBinOpRhs(move(rhs), precedence + 1);
            if (!rhs)
            {
                return nullptr;
            }
        }

        lhs = unique_ptr<Expr>(new BinaryOp(opToken, move(lhs), move(rhs)));
    }
}

unique_ptr<Identifier> Parser::parseIdentifier()
{
    expectToken(TokenKind::IDENTIFIER);
    auto expr = make_unique<Identifier>(m_currentToken.getStr());
    advance();
    return expr;
}

unique_ptr<Expr> Parser::parseIdExpression()
{
    auto id = parseIdentifier();

    unique_ptr<Expr> expr;
    // FunctionCall
    vector<unique_ptr<Expr>> arguments;
    if (m_currentToken.getKind() == TokenKind::OPEN_PAREN)
    {
        advance(); // ( token
        if (m_currentToken.getKind() != TokenKind::CLOSE_PAREN)
        {
            while (true)
            {
                arguments.push_back(parseExpression());
                if (m_currentToken.getKind() == TokenKind::CLOSE_PAREN)
                {
                    advance();
                    break;
                }
                if (m_currentToken.getKind() != TokenKind::COMMA)
                {
                    loge << "Unexpected token " << m_currentToken << ", expected COMMA";
                    throw runtime_error("Unexpected token");
                }
                advance();
            }
        }
        else
        {
            advance(); // ) token
        }
        expr = make_unique<FunctionCall>(move(id), move(arguments));
    }
    else
    {
        expr = move(id);
    }
    return expr;
}

unique_ptr<Expr> Parser::parseUnaryOperator()
{
    auto opToken = m_currentToken;
    advance(); // Operator
    if (opToken.getStr() == "-")
    {
        if (m_currentToken.getKind() == TokenKind::NUMBER)
        {
            unique_ptr<Expr> expr(new I32Literal(-stoi(m_currentToken.getStr().to_string())));
            advance();
            return expr;
        }
    }
    unique_ptr<Expr> expr(new UnaryOp(opToken, parseExpression()));
    advance();
    return expr;
}

unique_ptr<Expr> Parser::parseNumber()
{
    unique_ptr<Expr> expr(new I32Literal(stoi(m_currentToken.getStr().to_string())));
    advance();
    return expr;
}

std::unique_ptr<Expr> Parser::parseStringLiteral()
{
    auto tokenStr = m_currentToken.getStr();
    unique_ptr<Expr> expr(new StringLiteral(tokenStr.substr(1, tokenStr.size() - 2)));
    advance();
    return expr;
}

unique_ptr<Expr> Parser::parseFunctionDefinition()
{
    advance(); // FN token
    auto id = parseIdentifier();
    auto parameterPattern = parseTupleMatch();
    auto func = std::make_unique<Function>(move(id), move(parameterPattern));
    parseBlock(func->getBlock());
    return unique_ptr<Expr>(move(func));
}

unique_ptr<Expr> Parser::parseParenExpression()
{
    advance(); // OPEN_PAREN token
    auto expr = parseExpression();
    if (m_currentToken.getKind() != TokenKind::CLOSE_PAREN)
    {
        throw runtime_error("Expected CLOSE_PAREN token");
    }
    advance(); // CLOSE_PAREN token
    return expr;
}

unique_ptr<Expr> Parser::parseLetExpression()
{
    advance(); // LET token
    auto id = parseIdentifier();
    if (m_currentToken.getStr() != "=")
    {
        throw runtime_error("Expected EQUALS token");
    }
    advance(); // EQUALS token
    auto expr = parseExpression();

    return unique_ptr<Expr>(new LetExpr(move(id), move(expr)));
}

std::unique_ptr<Expr> Parser::parseIfExpression()
{
    advance(); // IF token
    auto condition = parseExpression();
    auto trueBlock = make_unique<Block>();
    parseBlock(*trueBlock);
    auto falseBlock = make_unique<Block>();
    if (m_currentToken.getKind() == TokenKind::ELSE)
    {
        advance(); // ELSE token
        parseBlock(*falseBlock);
    }
    return unique_ptr<Expr>(new IfExpr(move(condition), move(trueBlock), move(falseBlock)));
}

unique_ptr<Match> Parser::parseMatch()
{
    switch (m_currentToken.getKind())
    {
        case TokenKind::OPEN_PAREN:
            return parseTupleMatch();
        case TokenKind::IDENTIFIER:
            return parseIdMatch();
        default:
            return nullptr;
    }
}

unique_ptr<TupleMatch> Parser::parseTupleMatch()
{
    expectToken(TokenKind::OPEN_PAREN);
    advance(); // ( token
    vector<unique_ptr<Match>> subMatches;
    if (m_currentToken.getKind() != TokenKind::CLOSE_PAREN)
    {
        while (true)
        {
            subMatches.push_back(parseMatch());
            if (m_currentToken.getKind() == TokenKind::CLOSE_PAREN)
            {
                advance(); // CLOSE_PAREN
                break;
            }
            expectToken(TokenKind::COMMA);
            advance(); // COMMA
        }
    }
    return make_unique<TupleMatch>(move(subMatches));
}

unique_ptr<Match> Parser::parseIdMatch()
{
    auto id = parseIdentifier();
    unique_ptr<TypeMatch> typeMatch;
    if (m_currentToken.getKind() == TokenKind::COLON)
    {
        typeMatch = parseTypeMatch();
    }
    return make_unique<IdMatch>(move(id), move(typeMatch));
}

std::unique_ptr<TypeMatch> Parser::parseTypeMatch()
{
    auto typeId = parseIdentifier();
    return make_unique<TypeMatch>(move(typeId));
}

void Parser::expectToken(const TokenKind expected)
{
    if (m_currentToken.getKind() != expected)
    {
        ostringstream ss;
        ss << "Unexpected token " << m_currentToken << ", expected " << expected;
        throw runtime_error(ss.str());
    }
}

void Parser::advance()
{
    m_currentToken = move(m_nextToken);
    if (m_currentToken.getKind() != TokenKind::END_OF_INPUT)
    {
        m_nextToken = takeToken();
    }
}

Token Parser::takeToken()
{
    Token tok = m_lexer.take();
    while (!tok.isSignificant())
    {
        tok = m_lexer.take();
    }
    return tok;
}

}

