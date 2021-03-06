/**
 * Skiff Compiler
 */
#include "compiler.hpp"
#include "util/logger.hpp"
#include <iostream>
#include <string>

using sk::Compiler;
using std::cin;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "USAGE: skc [-s] file.sk" << endl;
        return 1;
    }

    //sk::setLogSeverity(sk::LogSeverity::WARN);

    logd << "Building compiler";
    auto* inFilename = argv[argc - 1];
    Compiler compiler(inFilename);

    logd << "compiling...";
    compiler.compile();

    logd << "done compiling";
    compiler.printAst(cout);
    cout << endl;

    if (argc > 2)
    {
        compiler.buildObjectFile();
    }
    else
    {
        compiler.buildLlFile();
    }
}
