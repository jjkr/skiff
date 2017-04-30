#pragma once
#include "util/string_view.hpp"
#include "source.hpp"
#include <ostream>
#include <cstdlib>

namespace sk
{
enum class TokenType
{
    WHITESPACE,
    NEWLINE,
    END_OF_INPUT,
    IDENTIFIER,
    NUMBER,

    COMMENT,

    FN,
    LET,
    IF,
    ELSE,
    FOR,
    WHILE,

    PLUS,
    TIMES,
    DIV,
    MINUS,
    EQUALS,

    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    COMMA
};

class Token
{
public:
    Token() = default;

    Token(TokenType type, string_view str, int line, int col)
        : m_type(type), m_str(str), m_line(line), m_col(col)
    {
    }

    TokenType getType() { return m_type; }
    string_view getStr() { return m_str; }
    int getLine() { return m_line; }
    int getCol() { return m_col; }

    bool isKeyword() const;
    bool isWhitespace() const;

private:
    TokenType m_type;
    string_view m_str;
    int m_line;
    int m_col;
};
int getTokenPrecedence(TokenType kind);
std::ostream& operator<<(std::ostream& os, Token token);


class Lexer
{
public:
    Lexer(string_view sourceStr) noexcept;
    Lexer(SourceBuffer& buffer) noexcept;

    Token takeToken();

private:
    const std::unique_ptr<SourceBuffer> m_bufferOwner;
    SourceBuffer& m_sourceBuffer;
    SourceBuffer::const_iterator m_sourceIter;

    void takeByte();
    Token makeToken(TokenType kind);

    //string_view m_text;

    // current token state
    uint32_t m_tokStart = 0u; // Source offset in bytes
    uint32_t m_tokSize = 0u;

    // byte is 0-based
    uint32_t m_byte = 0u;
    // line and col are 1-based
    int m_line = 1;
    int m_col = 1;

    // peek ahead one character
    char m_nextChar = '\0';
};

std::ostream& operator<<(std::ostream& os, TokenType kind);
}
