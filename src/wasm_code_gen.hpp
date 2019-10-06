#pragma once
#include "ast.hpp"
#include <binaryen-c.h>
#include <map>
#include <memory>

namespace sk
{
class BinaryenModuleRelease
{
public:
    BinaryenModuleRelease(const BinaryenModuleRef module) : m_module(module) {}
    ~BinaryenModuleRelease();

    BinaryenModuleRelease(const BinaryenModuleRelease&) = delete;
    BinaryenModuleRelease& operator=(const BinaryenModuleRelease&) = delete;

private:
    BinaryenModuleRef m_module;
};

class WasmCodeGen : public AstVisitor
{
public:
    WasmCodeGen();

    void visit(Module& module) override;
    void visit(Block& block) override;
    void visit(LetExpr& expr) override;
    void visit(Expr& expr) override;
    void visit(Function& func) override;
    void visit(FunctionCall& call) override;
    void visit(Identifier& variable) override;
    void visit(I32Literal& i32Literal) override;
    void visit(StringLiteral& str) override;
    void visit(BinaryOp& binOp) override;
    void visit(IfExpr& expr) override;
    void visit(Match& match) override;
    void visit(IdMatch& match) override;
    void visit(TupleMatch& match) override;
    void visit(TypeMatch& match) override;

    //std::map<string_view, llvm::Value*> m_symbols;
    //std::map<string_view, llvm::Value*> m_functions;
    //Block* m_block = nullptr;
    //llvm::Value* m_value = nullptr;
    //llvm::LLVMContext m_llvmContext;
    //llvm::IRBuilder<> m_irBuilder;
    //std::unique_ptr<llvm::Module> m_module;

    void printIr();

private:
    BinaryenModuleRef m_module;
    BinaryenModuleRelease m_moduleRelease;
};
}
