#pragma once
#include "util/string_view.hpp"
#include "source.hpp"
#include <ostream>
#include <cstdlib>

namespace sk
{
enum class TokenKind
{
    WHITESPACE,
    NEWLINE,
    END_OF_INPUT,
    IDENTIFIER,
    NUMBER,

    STRING_LITERAL,

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

    Token(TokenKind type, string_view str, int line, int col)
        : m_kind(type), m_str(str), m_line(line), m_col(col)
    {
    }

    TokenKind getKind() { return m_kind; }
    string_view getStr() { return m_str; }
    int getLine() { return m_line; }
    int getCol() { return m_col; }

    bool isKeyword() const;
    bool isWhitespace() const;

private:
    TokenKind m_kind;
    string_view m_str;
    int m_line;
    int m_col;
};
int getTokenPrecedence(TokenKind kind);
std::ostream& operator<<(std::ostream& os, Token token);


class Lexer
{
public:
    Lexer(string_view sourceStr) noexcept;
    Lexer(std::unique_ptr<SourceBuffer>&& buffer) noexcept;
    Lexer(SourceBuffer& buffer) noexcept;

    Token take();

private:
    const std::unique_ptr<SourceBuffer> m_bufferOwner;
    SourceBuffer& m_sourceBuffer;
    SourceBuffer::const_iterator m_sourceIter;

    char advance();
    Token makeToken(TokenKind kind);

    char currentChar();

    // current token state
    int m_tokStart = 0; // Source offset in bytes
    int m_tokSize = 0;

    // byte is 0-based
    int m_byte = 0;
    // line and col are 1-based
    int m_line = 1;
    int m_col = 1;
};

std::ostream& operator<<(std::ostream& os, TokenKind kind);
}
