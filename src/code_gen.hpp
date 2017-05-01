#pragma once
#include "ast.hpp"
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <map>
#include <memory>

namespace sk
{
class CodeGen : public AstVisitor
{
public:
    CodeGen(string_view sourceFile);

    void visit(Module& module) override;
    void visit(Block& block) override;
    void visit(LetExpr& expr) override;
    void visit(Expr& expr) override;
    void visit(Function& func) override;
    void visit(Identifier& variable) override;
    void visit(I32Literal& i32Literal) override;
    void visit(BinaryOp& binOp) override;

    llvm::Value* getValue() { return m_value; }
    llvm::Module& getLlvmModule() { return *m_module; }

private:
    std::map<string_view, llvm::Value*> m_symbols;
    Block* m_block = nullptr;
    llvm::Value* m_value = nullptr;
    llvm::LLVMContext m_llvmContext;
    llvm::IRBuilder<> m_irBuilder;
    std::unique_ptr<llvm::Module> m_module;
};
}