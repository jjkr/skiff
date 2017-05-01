#include "lexer.hpp"
#include "util/logger.hpp"
#include <sstream>
#include <stdexcept>
#include <memory>
#include <string>
#include <unordered_map>

using std::flush;
using std::hex;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::unordered_map;

namespace
{
using sk::Token;
using sk::TokenKind;
using sk::string_view;
const unordered_map<string_view, TokenKind> keywords = {
    {"fn", TokenKind::FN},
    {"let", TokenKind::LET},
    {"if", TokenKind::IF},
    {"else", TokenKind::ELSE},
    {"for", TokenKind::FOR},
    {"while", TokenKind::WHILE}
};

TokenKind identifierType(string_view id)
{
    const auto keyword = keywords.find(id);
    return keyword == keywords.cend() ? TokenKind::IDENTIFIER : keyword->second;
}
}

namespace sk
{
bool Token::isWhitespace() const
{
    switch (m_kind)
    {
        case TokenKind::WHITESPACE:
        case TokenKind::NEWLINE:
            return true;
        default:
            return false;
    }
}

int getTokenPrecedence(TokenKind tokenType)
{
    switch (tokenType)
    {
        case TokenKind::PLUS:
        case TokenKind::MINUS:
            return 10;
        case TokenKind::TIMES:
        case TokenKind::DIV:
            return 20;
        default:
            return -1;
    }
}

Lexer::Lexer(string_view sourceStr) noexcept
    : m_bufferOwner(SourceBuffer::fromSourceStr(sourceStr)),
      m_sourceBuffer(*m_bufferOwner),
      m_sourceIter(m_sourceBuffer.cbegin())
{
}

Lexer::Lexer(SourceBuffer& buffer) noexcept
    : m_bufferOwner(nullptr),
      m_sourceBuffer(buffer),
      m_sourceIter(m_sourceBuffer.cbegin())
{
}

Token Lexer::take()
{
    auto last = currentChar();
    if (last == '\0')
    {
        return makeToken(TokenKind::END_OF_INPUT);
    }
    auto current = advance();
    switch (last)
    {
        // newline
        case '\r':
        case '\n':
        {
            if ('\n' == current)
            {
                current = advance();
            }
            const auto tok = makeToken(TokenKind::NEWLINE);
            ++m_line;
            m_col = 1;
            return tok;
        }

        // Whitespace
        case ' ':
        case '\t':
        {
            while (current == ' ' || current == '\t')
            {
                current = advance();
            }
            return makeToken(TokenKind::WHITESPACE);
        }

        case '#':
        {
            // Single line comments
            while (current != '\r' && current != '\n' && current != '\0')
            {
                current = advance();
            }
            return makeToken(TokenKind::COMMENT);
        }

        // Parenthesis
        case '(':
            return makeToken(TokenKind::OPEN_PAREN);
        case ')':
            return makeToken(TokenKind::CLOSE_PAREN);
        case '{':
            return makeToken(TokenKind::OPEN_BRACE);
        case '}':
            return makeToken(TokenKind::CLOSE_BRACE);

        case ',':
            return makeToken(TokenKind::COMMA);

        // Operators
        case '+':
            return makeToken(TokenKind::PLUS);
        case '*':
            return makeToken(TokenKind::TIMES);
        case '/':
            return makeToken(TokenKind::DIV);
        case '-':
            return makeToken(TokenKind::MINUS);

        case '=':
            return makeToken(TokenKind::EQUALS);

        // Integer constants
        case '0' ... '9':
        {
            while (current >= '0' && current <= '9')
            {
                current = advance();
            }
            return makeToken(TokenKind::NUMBER);
        }

        // Identifiers
        case 'A' ... 'Z':
        case 'a' ... 'z':
        {
            while ((current >= 'A' && current <= 'Z') ||
                   (current >= 'a' && current <= 'z') ||
                   (current >= '0' && current <= '9'))
            {
                current = advance();
            }
            return makeToken(identifierType(m_sourceBuffer.getString(m_tokStart, m_tokSize)));
        }

        default:
            ostringstream ss;
            ss << "Unknown byte: " << hex << current;
            throw runtime_error(ss.str());
    }
}

char Lexer::advance()
{
    ++m_tokSize;
    ++m_byte;
    ++m_sourceIter;
    return currentChar();
    //m_nextChar = m_byte == m_text.size() ? '\0' : m_text[m_byte];
}

Token Lexer::makeToken(TokenKind tokenType)
{
    Token tok(tokenType, m_sourceBuffer.getString(m_tokStart, m_tokSize), m_line, m_col);
    m_col += m_tokSize;
    m_tokStart = m_byte;
    m_tokSize = 0;

    logi << "makeToken returning: " << tok;
    return tok;
}

char Lexer::currentChar()
{
    if (m_sourceIter != m_sourceBuffer.cend())
    {
        return *m_sourceIter;
    }
    else
    {
        return '\0';
    }
}

ostream& operator<<(ostream& os, Token token)
{
    return os << token.getKind() << " '" << token.getStr() << "' [" << token.getLine()
              << ", " << token.getCol() << "] ";
}

ostream& operator<<(ostream& os, TokenKind tokenType)
{
    switch (tokenType)
    {
        case TokenKind::WHITESPACE:
            os << "WHITESPACE";
            break;
        case TokenKind::NEWLINE:
            os << "NEWLINE";
            break;
        case TokenKind::END_OF_INPUT:
            os << "END_OF_INPUT";
            break;
        case TokenKind::IDENTIFIER:
            os << "IDENTIFIER";
            break;
        case TokenKind::COMMENT:
            os << "COMMENT";
            break;
        case TokenKind::NUMBER:
            os << "NUMBER";
            break;
        case TokenKind::FN:
            os << "FN";
            break;
        case TokenKind::LET:
            os << "LET";
            break;
        case TokenKind::IF:
            os << "IF";
            break;
        case TokenKind::ELSE:
            os << "ELSE";
            break;
        case TokenKind::FOR:
            os << "FOR";
            break;
        case TokenKind::WHILE:
            os << "WHILE";
            break;
        case TokenKind::PLUS:
            os << "PLUS";
            break;
        case TokenKind::MINUS:
            os << "MINUS";
            break;
        case TokenKind::TIMES:
            os << "TIMES";
            break;
        case TokenKind::DIV:
            os << "DIV";
            break;
        case TokenKind::EQUALS:
            os << "EQUALS";
            break;
        case TokenKind::OPEN_PAREN:
            os << "OPEN_PAREN";
            break;
        case TokenKind::CLOSE_PAREN:
            os << "CLOSE_PAREN";
            break;
        case TokenKind::OPEN_BRACE:
            os << "OPEN_BRACE";
            break;
        case TokenKind::CLOSE_BRACE:
            os << "CLOSE_BRACE";
            break;
        case TokenKind::COMMA:
            os << "COMMA";
            break;
    }

    return os;
}
}
