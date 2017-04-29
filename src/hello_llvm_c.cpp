#include "util/logger.hpp"
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <cstdlib>

int main(int argc, char** argv)
{
    auto module = LLVMModuleCreateWithName("hello");

    LLVMTypeRef paramTypes[] = {LLVMInt32Type(), LLVMInt32Type()};
    auto retType = LLVMFunctionType(LLVMInt32Type(), paramTypes, 2, false);
    auto sumFunc = LLVMAddFunction(module, "sum", retType);

    auto entry = LLVMAppendBasicBlock(sumFunc, "entry");

    auto builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sumFunc, 0), LLVMGetParam(sumFunc, 1),
                                    "tmp");
    LLVMBuildRet(builder, tmp);

    char *error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    error = NULL;
    //LLVMLinkInJIT();
    LLVMInitializeNativeTarget();
    if (LLVMCreateExecutionEngineForModule(&engine, module, &error) != 0) {
        loge << "failed to create execution engine: " << error;
        abort();
    }
    if (error) {
        loge << "error: " << error;
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    if (argc < 3) {
        fprintf(stderr, "usage: %s x y\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    long long x = strtoll(argv[1], NULL, 10);
    long long y = strtoll(argv[2], NULL, 10);

    LLVMGenericValueRef args[] = {
        LLVMCreateGenericValueOfInt(LLVMInt32Type(), x, 0),
        LLVMCreateGenericValueOfInt(LLVMInt32Type(), y, 0)
    };
    LLVMGenericValueRef res = LLVMRunFunction(engine, sumFunc, 2, args);
    printf("%d\n", (int)LLVMGenericValueToInt(res, 0));

    // Write out bitcode to file
    if (LLVMWriteBitcodeToFile(module, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);

}