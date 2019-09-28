/**
 * Skiff Interpreter
 */
#include "ast.hpp"
#include "code_gen.hpp"
#include "lexer.hpp"
#include "ast_printer.hpp"
#include "source.hpp"
#include "parser.hpp"
#include "util/logger.hpp"
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <string>
#include <memory>

using sk::AstPrinter;
using sk::CodeGen;
using sk::Module;
using sk::Lexer;
using sk::Token;
using sk::Parser;
using sk::SourceBuffer;
using std::getline;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::make_unique;
using std::string;

int main(int argc, char** argv)
{
    cout << "ski> " << flush;

    auto sourceBuffer = make_unique<SourceBuffer>();
    Lexer lexer(*sourceBuffer);
    Module module("ski");
    Parser parser(module, lexer);

    for (string line; getline(cin, line);)
    {
        logi << "parsing input: " << line;

        sourceBuffer->addBlock(line);
        parser.parse();

        AstPrinter printer(cout);
        printer.dispatch(module);

        CodeGen codeGen("ski");
        codeGen.dispatch(module);
        codeGen.getLlvmModule().print(llvm::outs(), nullptr);
        cout << "ski> " << flush;
    }
}
