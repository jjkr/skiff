#include "code_gen.hpp"
#include "util/logger.hpp"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

using llvm::ConstantInt;
using llvm::Type;

namespace sk
{
CodeGen::CodeGen() : m_irBuilder(m_llvmContext),
                     m_module(new llvm::Module("main", m_llvmContext)) {}

void CodeGen::visit(Module& module)
{
    logi << "Codegen::visit module";
}

void CodeGen::visit(Expr& expr)
{
    logi << "Codegen::visit expr";
}

void CodeGen::visit(Variable& variable)
{
    logi << "Codegen::visit variable";
}

void CodeGen::visit(I32Literal& lit)
{
    logi << "Codegen::visit i32 literal";
    m_value = ConstantInt::getSigned(Type::getInt32Ty(m_llvmContext), lit.getValue());
}

void CodeGen::visit(BinaryOp& binOp)
{
    logi << "Codegen::visit binary op";
    binOp.getLhs()->accept(*this);
    auto lhs = m_value;
    binOp.getRhs()->accept(*this);
    auto rhs = m_value;

    auto opType = binOp.getType();
    switch (opType)
    {
        case TokenType::PLUS:
            m_value = m_irBuilder.CreateAdd(lhs, rhs);
            break;
        case TokenType::MINUS:
            m_value = m_irBuilder.CreateSub(lhs, rhs);
            break;
        case TokenType::TIMES:
            m_value = m_irBuilder.CreateMul(lhs, rhs);
            break;
        case TokenType::DIV:
            m_value = m_irBuilder.CreateSDiv(lhs, rhs);
            break;
        default:
            loge << "unknown operator: " << opType;
    }
    //m_value->dump();
}
}
