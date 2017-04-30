#include "compiler.hpp"
#include "ast_printer.hpp"
#include "util/logger.hpp"
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <regex>

using sk::AstPrinter;
using sk::CodeGen;
using sk::Lexer;
using sk::Module;
using sk::Token;
using sk::SourceBuffer;
using sk::Parser;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::regex;
using std::regex_replace;
using std::string;

namespace sk
{
Compiler::Compiler(const char* filename)
    : m_filename(filename),
      m_source(SourceBuffer::readFile(filename)),
      m_lexer(*m_source),
      m_module(filename),
      m_parser(m_module, m_lexer),
      m_codeGen(filename)
{
}

void Compiler::compile()
{
    m_parser.parse();
    m_codeGen.dispatch(m_module);
}

void Compiler::printAst(ostream& os)
{
    AstPrinter printer(os);
    printer.dispatch(m_module);
}

void Compiler::buildObjectFile()
{
    // Write .o files
    auto objFilename = regex_replace(m_filename, regex("sk$"), "o");

    std::error_code err;
    llvm::raw_fd_ostream outFile(objFilename, err, llvm::sys::fs::F_RW);
    if (err)
    {
        ostringstream ss;
        ss << "Failed to open file: " << err.message();
        throw runtime_error(ss.str());
    }

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    string targetLookupError;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, targetLookupError);
    if (!target)
    {
        loge << targetLookupError;
        throw runtime_error("Failed to lookup target triple");
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions options;
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, CPU, Features, options, rm);

    m_codeGen.getLlvmModule().setDataLayout(targetMachine->createDataLayout());
    m_codeGen.getLlvmModule().setTargetTriple(targetTriple);

    llvm::legacy::PassManager pass;
    auto fileType = llvm::TargetMachine::CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, outFile, fileType))
    {
        throw runtime_error("TargetMachine can't emit a file of this type");
    }

    pass.run(m_codeGen.getLlvmModule());
    outFile.flush();
}

void Compiler::buildLlFile()
{
    auto outFilename = regex_replace(m_filename, regex("sk$"), "ll");

    std::error_code err;
    llvm::raw_fd_ostream outFile(outFilename, err, llvm::sys::fs::F_RW);
    if (err)
    {
        ostringstream ss;
        ss << "Failed to open file: " << err.message();
        throw runtime_error(ss.str());
    }

    m_codeGen.getLlvmModule().print(outFile, nullptr);

}
}