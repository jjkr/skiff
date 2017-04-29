/**
 * Skiff Compiler
 */
#include "code_gen.hpp"
#include "lexer.hpp"
#include "ast_printer.hpp"
#include "parser.hpp"
#include "util/logger.hpp"
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <string>

using sk::AstPrinter;
using sk::CodeGen;
using sk::Lexer;
using sk::Token;
using sk::Parser;
using std::getline;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::string;

int main(int argc, char** argv)
{
    cout << "ski> " << flush;

    for (string line; getline(cin, line);)
    {
        CodeGen codeGen;
        Lexer lexer(line);
        Parser parser(lexer);

        cout << "parsing input: " << line << endl;
        auto expr = parser.parse();

        if (expr)
        {
            AstPrinter printer(cout);
            expr->accept(printer);

            expr->accept(codeGen);
            codeGen.getModule().print(llvm::outs(), nullptr);
        }
        cout << "ski> " << flush;
    }
}
