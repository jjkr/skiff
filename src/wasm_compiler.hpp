#pragma once
#include "wasm_code_gen.hpp"
#include "source.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "util/string_view.hpp"
#include <ostream>
#include <memory>

namespace sk
{
class WasmCompiler
{
public:
    WasmCompiler(const char* filename);

    void compile();
    void printAst(std::ostream& out);
    void printIr();

private:
    const char* const m_filename;
    SourceBuffer m_source;
    Lexer m_lexer;
    Module m_module;
    Parser m_parser;
    WasmCodeGen m_codeGen;
};
}
