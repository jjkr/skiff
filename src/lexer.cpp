#include "lexer.hpp"
#include "util/logger.hpp"
#include <sstream>
#include <stdexcept>
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
const unordered_map<string, Token::Kind> keywords = {
    {"fn", Token::Kind::FN},
    {"if", Token::Kind::IF},
    {"else", Token::Kind::ELSE},
    {"for", Token::Kind::FOR},
    {"while", Token::Kind::WHILE}
};

Token::Kind identifierKind(const string& id)
{
    const auto keyword = keywords.find(id);
    return keyword == keywords.cend() ? Token::Kind::IDENTIFIER : keyword->second;
}
}

namespace sk
{
bool Token::isWhitespace() const
{
    switch (kind)
    {
        case Token::Kind::WHITESPACE:
        case Token::Kind::NEWLINE:
            return true;
        default:
            return false;
    }
}

int getTokenPrecedence(Token::Kind kind)
{
    switch (kind)
    {
        case Token::Kind::PLUS:
        case Token::Kind::MINUS:
            return 10;
        case Token::Kind::TIMES:
        case Token::Kind::DIV:
            return 20;
        default:
            return -1;
    }
}

Lexer::Lexer(string_view text) noexcept : m_text(text), m_tokStart(&text[0])
{

    //logger.debug().write(&text[0], text.size());

    if (m_text.size() > 0)
    {
        m_nextChar = m_text[0];
    }
}

Token Lexer::takeToken()
{
    if (m_byte == m_text.size())
    {
        return makeToken(Token::Kind::END_OF_INPUT);
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
            const auto tok = makeToken(Token::Kind::NEWLINE);
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
            return makeToken(Token::Kind::WHITESPACE);
        }

        // Parenthesis
        case '(':
            return makeToken(Token::Kind::OPEN_PAREN);
        case ')':
            return makeToken(Token::Kind::CLOSE_PAREN);
        case '{':
            return makeToken(Token::Kind::OPEN_BRACE);
        case '}':
            return makeToken(Token::Kind::CLOSE_BRACE);

        // Operators
        case '+':
            return makeToken(Token::Kind::PLUS);
        case '*':
            return makeToken(Token::Kind::TIMES);
        case '/':
            return makeToken(Token::Kind::DIV);
        case '-':
            return makeToken(Token::Kind::MINUS);

        // Integer constants
        case '0' ... '9':
        {
            while (m_nextChar >= '0' && m_nextChar <= '9')
            {
                takeByte();
            }
            return makeToken(Token::Kind::NUMBER);
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
            return makeToken(identifierKind({m_tokStart, m_tokSize}));
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
    m_nextChar = m_byte == m_text.size() ? '\0' : m_text[m_byte];
}

Token Lexer::makeToken(Token::Kind kind)
{
    Token tok(kind, {m_tokStart, m_tokSize}, m_line, m_col);
    m_col += m_tokSize;
    m_tokStart = &m_text[m_byte];
    m_tokSize = 0;

    logi << "makeToken returning: " << tok;
    return tok;
}

ostream& operator<<(ostream& os, Token token)
{
    return os << token.kind << " '" << token.str << "' [" << token.line
              << ", " << token.col << "] ";
}

ostream& operator<<(ostream& os, Token::Kind kind)
{
    switch (kind)
    {
        case Token::Kind::WHITESPACE:
            os << "WHITESPACE";
            break;
        case Token::Kind::NEWLINE:
            os << "NEWLINE";
            break;
        case Token::Kind::END_OF_INPUT:
            os << "END_OF_INPUT";
            break;
        case Token::Kind::IDENTIFIER:
            os << "IDENTIFIER";
            break;
        case Token::Kind::NUMBER:
            os << "NUMBER";
            break;
        case Token::Kind::FN:
            os << "FN";
            break;
        case Token::Kind::IF:
            os << "IF";
            break;
        case Token::Kind::ELSE:
            os << "ELSE";
            break;
        case Token::Kind::FOR:
            os << "FOR";
            break;
        case Token::Kind::WHILE:
            os << "WHILE";
            break;
        case Token::Kind::PLUS:
            os << "PLUS";
            break;
        case Token::Kind::MINUS:
            os << "MINUS";
            break;
        case Token::Kind::TIMES:
            os << "TIMES";
            break;
        case Token::Kind::DIV:
            os << "DIV";
            break;
        case Token::Kind::OPEN_PAREN:
            os << "OPEN_PAREN";
            break;
        case Token::Kind::CLOSE_PAREN:
            os << "CLOSE_PAREN";
            break;
        case Token::Kind::OPEN_BRACE:
            os << "OPEN_BRACE";
            break;
        case Token::Kind::CLOSE_BRACE:
            os << "CLOSE_BRACE";
            break;
    }

    return os;
}
}
