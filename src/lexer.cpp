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
using sk::TokenType;
using sk::string_view;
const unordered_map<string_view, TokenType> keywords = {
    {"fn", TokenType::FN},
    {"let", TokenType::LET},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE}
};

TokenType identifierType(string_view id)
{
    const auto keyword = keywords.find(id);
    return keyword == keywords.cend() ? TokenType::IDENTIFIER : keyword->second;
}
}

namespace sk
{
bool Token::isWhitespace() const
{
    switch (m_type)
    {
        case TokenType::WHITESPACE:
        case TokenType::NEWLINE:
            return true;
        default:
            return false;
    }
}

int getTokenPrecedence(TokenType tokenType)
{
    switch (tokenType)
    {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 10;
        case TokenType::TIMES:
        case TokenType::DIV:
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
    if (m_sourceIter != m_sourceBuffer.cend())
    {
        m_nextChar = *m_sourceIter;
    }
}

Lexer::Lexer(SourceBuffer& buffer) noexcept
    : m_bufferOwner(nullptr),
      m_sourceBuffer(buffer),
      m_sourceIter(m_sourceBuffer.cbegin())
{
    if (m_sourceIter != m_sourceBuffer.cend())
    {
        m_nextChar = *m_sourceIter;
    }
}

Token Lexer::takeToken()
{
    if (m_nextChar == '\0')
    {
        return makeToken(TokenType::END_OF_INPUT);
    }
    const auto current = m_nextChar;
    takeByte();
    switch (current)
    {
        // newline
        case '\r':
        case '\n':
        {
            if ('\n' == m_nextChar)
            {
                takeByte();
            }
            const auto tok = makeToken(TokenType::NEWLINE);
            ++m_line;
            m_col = 1;
            return tok;
        }

        // Whitespace
        case ' ':
        case '\t':
        {
            while (m_nextChar == ' ' || m_nextChar == '\t')
            {
                takeByte();
            }
            return makeToken(TokenType::WHITESPACE);
        }

        case '#':
        {
            // Single line comments
            while (m_nextChar != '\r' && m_nextChar != '\n' && m_nextChar != '\0')
            {
                takeByte();
            }
            return makeToken(TokenType::COMMENT);
        }

        // Parenthesis
        case '(':
            return makeToken(TokenType::OPEN_PAREN);
        case ')':
            return makeToken(TokenType::CLOSE_PAREN);
        case '{':
            return makeToken(TokenType::OPEN_BRACE);
        case '}':
            return makeToken(TokenType::CLOSE_BRACE);

        case ',':
            return makeToken(TokenType::COMMA);

        // Operators
        case '+':
            return makeToken(TokenType::PLUS);
        case '*':
            return makeToken(TokenType::TIMES);
        case '/':
            return makeToken(TokenType::DIV);
        case '-':
            return makeToken(TokenType::MINUS);

        case '=':
            return makeToken(TokenType::EQUALS);

        // Integer constants
        case '0' ... '9':
        {
            while (m_nextChar >= '0' && m_nextChar <= '9')
            {
                takeByte();
            }
            return makeToken(TokenType::NUMBER);
        }

        // Identifiers
        case 'A' ... 'Z':
        case 'a' ... 'z':
        {
            while ((m_nextChar >= 'A' && m_nextChar <= 'Z') ||
                   (m_nextChar >= 'a' && m_nextChar <= 'z') ||
                   (m_nextChar >= '0' && m_nextChar <= '9'))
            {
                takeByte();
            }
            return makeToken(identifierType(m_sourceBuffer.getString(m_tokStart, m_tokSize)));
        }

        default:
            ostringstream ss;
            ss << "Unknown byte: " << hex << current;
            throw runtime_error(ss.str());
    }
}

void Lexer::takeByte()
{
    ++m_tokSize;
    ++m_byte;
    ++m_sourceIter;
    if (m_sourceIter == m_sourceBuffer.cend())
    {
        m_nextChar = '\0';
    }
    else
    {
        m_nextChar = *m_sourceIter;
    }
    //m_nextChar = m_byte == m_text.size() ? '\0' : m_text[m_byte];
}

Token Lexer::makeToken(TokenType tokenType)
{
    Token tok(tokenType, m_sourceBuffer.getString(m_tokStart, m_tokSize), m_line, m_col);
    m_col += m_tokSize;
    m_tokStart = m_byte;
    m_tokSize = 0;

    logi << "makeToken returning: " << tok;
    return tok;
}

ostream& operator<<(ostream& os, Token token)
{
    return os << token.getType() << " '" << token.getStr() << "' [" << token.getLine()
              << ", " << token.getCol() << "] ";
}

ostream& operator<<(ostream& os, TokenType tokenType)
{
    switch (tokenType)
    {
        case TokenType::WHITESPACE:
            os << "WHITESPACE";
            break;
        case TokenType::NEWLINE:
            os << "NEWLINE";
            break;
        case TokenType::END_OF_INPUT:
            os << "END_OF_INPUT";
            break;
        case TokenType::IDENTIFIER:
            os << "IDENTIFIER";
            break;
        case TokenType::COMMENT:
            os << "COMMENT";
            break;
        case TokenType::NUMBER:
            os << "NUMBER";
            break;
        case TokenType::FN:
            os << "FN";
            break;
        case TokenType::LET:
            os << "LET";
            break;
        case TokenType::IF:
            os << "IF";
            break;
        case TokenType::ELSE:
            os << "ELSE";
            break;
        case TokenType::FOR:
            os << "FOR";
            break;
        case TokenType::WHILE:
            os << "WHILE";
            break;
        case TokenType::PLUS:
            os << "PLUS";
            break;
        case TokenType::MINUS:
            os << "MINUS";
            break;
        case TokenType::TIMES:
            os << "TIMES";
            break;
        case TokenType::DIV:
            os << "DIV";
            break;
        case TokenType::EQUALS:
            os << "EQUALS";
            break;
        case TokenType::OPEN_PAREN:
            os << "OPEN_PAREN";
            break;
        case TokenType::CLOSE_PAREN:
            os << "CLOSE_PAREN";
            break;
        case TokenType::OPEN_BRACE:
            os << "OPEN_BRACE";
            break;
        case TokenType::CLOSE_BRACE:
            os << "CLOSE_BRACE";
            break;
        case TokenType::COMMA:
            os << "COMMA";
            break;
    }

    return os;
}
}
