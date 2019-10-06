/**
 * Ski Compiler
 */
#include "wasm_compiler.hpp"
#include "util/logger.hpp"
#include <iostream>
#include <string>

using sk::WasmCompiler;
using std::cin;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "USAGE: skic [-s] file.sk" << endl;
        return 1;
    }

    //sk::setLogSeverity(sk::LogSeverity::WARN);

    logd << "Building compiler";
    auto* inFilename = argv[argc - 1];
    WasmCompiler compiler(inFilename);

    logd << "compiling...";
    compiler.compile();

    logd << "done compiling";
    compiler.printAst(cout);
    cout << endl;

    logd << "**** BINARYEN IR MODULE ****";
    compiler.printIr();
}
