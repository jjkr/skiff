/**
 * Skiff Compiler
 */
#include "ast.hpp"
#include "code_gen.hpp"
#include "lexer.hpp"
#include "ast_printer.hpp"
#include "source.hpp"
#include "parser.hpp"
#include "util/logger.hpp"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <regex>

using sk::AstPrinter;
using sk::CodeGen;
using sk::Lexer;
using sk::Module;
using sk::Token;
using sk::SourceBuffer;
using sk::Parser;
using std::fopen;
using std::fread;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::regex;
using std::regex_replace;
using std::string;

constexpr int FILE_BUFFER_SIZE = 4 * 1024 * 1024;
char FILE_BUFFER[FILE_BUFFER_SIZE];

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "USAGE: skc [-s] file.sk" << endl;
        return 1;
    }

    auto* inFilename = argv[argc - 1];
    SourceBuffer src = SourceBuffer::readFile(inFilename);

    Lexer lexer(src);
    auto moduleName = regex_replace(inFilename, regex("\\.sk$"), "");
    logi << "moduleName: " << moduleName;
    Module module(moduleName);
    Parser parser(module, lexer);

    parser.parse();

    AstPrinter printer(cout);
    printer.dispatch(module);

    CodeGen codeGen(inFilename);
    codeGen.dispatch(module);

    if (argc > 2)
    {
        // Write .o files
        auto objFilename = regex_replace(inFilename, regex("sk$"), "o");

        std::error_code err;
        llvm::raw_fd_ostream outFile(objFilename, err, llvm::sys::fs::F_RW);
        if (err)
        {
            loge << "Failed to open file: " << err.message();
            return 1;
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
            return 1;
        }

        auto CPU = "generic";
        auto Features = "";

        llvm::TargetOptions options;
        auto rm = llvm::Optional<llvm::Reloc::Model>();
        auto targetMachine = target->createTargetMachine(targetTriple, CPU, Features, options, rm);

        codeGen.getLlvmModule().setDataLayout(targetMachine->createDataLayout());
        codeGen.getLlvmModule().setTargetTriple(targetTriple);

        llvm::legacy::PassManager pass;
        auto fileType = llvm::TargetMachine::CGFT_ObjectFile;

        if (targetMachine->addPassesToEmitFile(pass, outFile, fileType))
        {
            loge << "TargetMachine can't emit a file of this type";
            return 1;
        }

        pass.run(codeGen.getLlvmModule());
        outFile.flush();
    }
    else
    {
        auto outFilename = regex_replace(inFilename, regex("sk$"), "ll");

        std::error_code err;
        llvm::raw_fd_ostream outFile(outFilename, err, llvm::sys::fs::F_RW);
        if (err)
        {
            loge << "Failed to open file: " << err.message();
            return 1;
        }

        codeGen.getLlvmModule().print(outFile, nullptr);
    }
}
