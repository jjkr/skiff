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

bool isOperatorChar(char c)
{
    switch (c)
    {
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '<':
        case '>':
        case '@':
        case '$':
        case '~':
        case '&':
        case '%':
        case '|':
        case '!':
        case '?':
        case '^':
        case '.':
        case ':':
        case '\\':
            return true;
        default:
            return false;
    }
}

using sk::Token;
using sk::TokenKind;
using sk::string_view;
const unordered_map<string_view, TokenKind> keywords = {
    {"fn", TokenKind::FN},
    {"let", TokenKind::LET},
    {"if", TokenKind::IF},
    {"else", TokenKind::ELSE},
    {"for", TokenKind::FOR},
    {"and", TokenKind::AND},
    {"or", TokenKind::OR},
    {"not", TokenKind::NOT},
    {"xor", TokenKind::XOR},
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
/*
 * Precedence level | Operators                                  | First character | Terminal symbol
 *   10 (highest)   |                                            | $ ^             |  OP10
 *   9              | * / div mod shl shr %                      | * % \ /         |  OP9
 *   8              | + -                                        | + - ~ |         |  OP8
 *   7              | &                                          | &               |  OP7
 *   6              | ..                                         | .               |  OP6
 *   5              | == <= < >= > != in notin is isnot not of   | = < > !         |  OP5
 *   4              | and                                        |                 |  OP4
 *   3              | or xor                                     |                 |  OP3
 *   2              |                                            | @ : ?           |  OP2
 *   1              | assignment operator (like +=, *=)          |                 |  OP1
 *   0 (lowest)     | arrow like operator (like ->, =>)          |                 |  OP0
 * @param token
 * @return
 */
int getTokenPrecedence(Token& token)
{
    if (token.getKind() != TokenKind::OPERATOR)
    {
        return -1;
    }
    switch (token.getStr()[0])
    {
        case '$':
        case '^':
            return 10;
        case '*':
        case '/':
        case '%':
        case '\\':
            return 9;
        case '+':
        case '-':
        case '~':
        case '|':
            return 8;
        case '&':
            return 7;
        case '.':
            return 6;
        case '=':
        case '<':
        case '>':
        case '!':
            return 5;
        case '@':
        case ':':
        case '?':
            return 2;
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
    auto current = currentChar();
    if (current == '\0')
    {
        return makeToken(TokenKind::END_OF_INPUT);
    }

    auto next = advance();
    switch (current)
    {
        // newline
        case '\r':
        case '\n':
        {
            if (current == '\r' && '\n' == next)
            {
                advance();
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
            while (next == ' ' || next == '\t')
            {
                next = advance();
            }
            return makeToken(TokenKind::WHITESPACE);
        }

        case '#':
        {
            // Single line comments
            while (next != '\r' && next != '\n' && next != '\0')
            {
                next = advance();
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
        case '[':
            return makeToken(TokenKind::OPEN_BRACKET);
        case ']':
            return makeToken(TokenKind::CLOSE_BRACKET);

        case ',':
            return makeToken(TokenKind::COMMA);

        //
        // Operators are any combination of these characters
        //      =    +     -     *     /     <     >
        //      @     $     ~     &     %     |
        //      !     ?     ^     .     :     \
        //
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '<':
        case '>':
        case '@':
        case '$':
        case '~':
        case '&':
        case '%':
        case '|':
        case '!':
        case '?':
        case '^':
        case '.':
        case ':':
        case '\\':
        {
            while (isOperatorChar(next))
            {
                next = advance();
            }
            return makeToken(TokenKind::OPERATOR);
        }

        // Integer constants
        case '0' ... '9':
        {
            while (next >= '0' && next <= '9')
            {
                next = advance();
            }
            return makeToken(TokenKind::NUMBER);
        }

        // String literal
        case '\'':
        {
            while (next != '\'')
            {
                next = advance();
            }
            return makeToken(TokenKind::STRING_LITERAL);
        }
        case '\"':
        {
            while (next != '\"')
            {
                next = advance();
            }
            advance();
            return makeToken(TokenKind::STRING_LITERAL);
        }

        // Identifiers
        case 'A' ... 'Z':
        case 'a' ... 'z':
        {
            while ((next >= 'A' && next <= 'Z') ||
                   (next >= 'a' && next <= 'z') ||
                   (next >= '0' && next <= '9'))
            {
                next = advance();
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
        case TokenKind::STRING_LITERAL:
            os << "STRING_LITERAL";
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
        case TokenKind::OPERATOR:
            os << "OPERATOR";
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
        case TokenKind::OPEN_BRACKET:
            os << "OPEN_BRACKET";
            break;
        case TokenKind::CLOSE_BRACKET:
            os << "CLOSE_BRACKET";
            break;
        case TokenKind::COLON:
            os << "COLON";
        case TokenKind::COMMA:
            os << "COMMA";
            break;
    }

    return os;
}
}
