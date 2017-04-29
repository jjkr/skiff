#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <memory>
#include <vector>

using std::cout;
using std::make_unique;
using std::vector;

int main()
{
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> irBuilder(llvmContext);

    auto module = llvm::make_unique<llvm::Module>("main", llvmContext);

    auto funcName = "addFive";

    //vector<llvm::Type*> argList = {llvm::Type::getInt32Ty(llvmContext)};
    //auto funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvmContext),
    //                                        argList, false);
    //auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcName,
    //                                   module.get());
    //func->args().begin()->setName("x");

    //auto bb = llvm::BasicBlock::Create(llvmContext, "entry", func);
    //irBuilder.SetInsertPoint(bb);

    //auto fiveLiteral = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvmContext),
    //                                          llvm::APInt(32, 5));
    //auto addExpr = irBuilder.CreateAdd(&*(func->args().begin()), fiveLiteral, "result");
    //irBuilder.CreateRet(addExpr);

    module->print(llvm::errs(), nullptr);
}