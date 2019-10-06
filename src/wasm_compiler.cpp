#include "wasm_compiler.hpp"
#include "ast_printer.hpp"
#include "util/logger.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
#include <regex>

using std::ostream;

namespace sk
{
WasmCompiler::WasmCompiler(const char* filename)
    : m_filename(filename),
      m_source(SourceBuffer::readFile(filename)),
      m_lexer(m_source),
      m_module(filename),
      m_parser(m_module, m_lexer),
      m_codeGen()
{
}

void WasmCompiler::compile()
{
    m_parser.parse();
    m_codeGen.dispatch(m_module);
}

void WasmCompiler::printAst(ostream& os)
{
    AstPrinter printer(os);
    printer.dispatch(m_module);
}

void WasmCompiler::printIr()
{
    m_codeGen.printIr();
}
}
