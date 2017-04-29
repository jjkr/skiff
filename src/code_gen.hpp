#pragma once
#include "ast.hpp"
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <memory>

namespace sk
{
class CodeGen : public AstVisitor
{
public:
    CodeGen();

    void visit(Module& module) override;
    void visit(Block& block) override;
    void visit(Expr& expr) override;
    void visit(Function& func) override;
    void visit(Variable& variable) override;
    void visit(I32Literal& i32Literal) override;
    void visit(BinaryOp& binOp) override;

    llvm::Value* getValue() { return m_value; }
    llvm::Module& getModule() { return *m_module; }

    void dump() { }

private:
    llvm::Value* m_value = nullptr;
    llvm::LLVMContext m_llvmContext;
    llvm::IRBuilder<> m_irBuilder;
    std::unique_ptr<llvm::Module> m_module;
};
}