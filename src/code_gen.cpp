#include "code_gen.hpp"
#include "util/logger.hpp"
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

using llvm::ConstantInt;
using llvm::Type;
using std::make_unique;
using std::move;
using std::ostringstream;
using std::runtime_error;
using std::vector;

namespace sk
{
CodeGen::CodeGen(string_view sourceFile)
    : m_irBuilder(m_llvmContext),
      m_module(new llvm::Module(llvm::StringRef(sourceFile.data(), sourceFile.size()),
                                m_llvmContext))
{
}

void CodeGen::visit(Module& module)
{
    logi << "Codegen::visit module";

    vector<llvm::Type*> parameterList = {
        llvm::Type::getInt32Ty(m_llvmContext),
        llvm::PointerType::get(llvm::Type::getInt8PtrTy(m_llvmContext), 0)};
    auto funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_llvmContext),
                                            move(parameterList), false);
    auto llvmFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main",
                                           m_module.get());
    auto arg = llvmFunc->args().begin();
    arg->setName("argc");
    ++arg;
    arg->setName("argv");

    auto bb = llvm::BasicBlock::Create(m_llvmContext, "", llvmFunc);
    m_irBuilder.SetInsertPoint(bb);
    dispatch(module.getMainBlock());

    m_irBuilder.SetInsertPoint(bb);
    m_irBuilder.CreateRet(m_value);
}

void CodeGen::visit(Block& block)
{
    logi << "Codegen::visit block";
    for (auto& e : block.getExpressions())
    {
        dispatch(e);
    }
}

void CodeGen::visit(LetExpr& letExpr)
{
    logi << "Codegen::visit let";
    dispatch(letExpr.getExpr());
    m_symbols[letExpr.getIdentifier().getName()] = m_value;
}

void CodeGen::visit(Expr& expr)
{
    logi << "Codegen::visit expr";
}

void CodeGen::visit(Function& func)
{
    logi << "Codegen::visit func";
    vector<llvm::Type*> parameterList;
    for (auto i = 0u; i < func.getParameters().size(); ++i)
    {
        parameterList.push_back(llvm::Type::getInt32Ty(m_llvmContext));
    }
    auto funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_llvmContext),
                                            move(parameterList), false);
    auto funcName = func.getName();
    auto llvmFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                           llvm::StringRef(funcName.data(), funcName.size()),
                                           m_module.get());
    auto arg = llvmFunc->args().begin();
    for (auto i = 0u; i < func.getParameters().size(); ++i, ++arg)
    {
        auto paramName = func.getParameters()[i];
        arg->setName(llvm::StringRef(paramName.data(), paramName.size()));
        m_symbols[paramName] = &*arg;
    }

    auto bb = llvm::BasicBlock::Create(m_llvmContext, "entry", llvmFunc);
    m_irBuilder.SetInsertPoint(bb);

    dispatch(func.getBlock());

    m_irBuilder.CreateRet(m_value);
}

void CodeGen::visit(Variable& variable)
{
    logi << "Codegen::visit variable";
    auto val = m_symbols.find(variable.getName());
    if (val == m_symbols.end())
    {
        ostringstream ss;
        ss << "Symbol not found: " << variable.getName();
        throw runtime_error(ss.str());
    }
    m_value = val->second;
}

void CodeGen::visit(I32Literal& lit)
{
    logi << "Codegen::visit i32 literal";
    m_value = ConstantInt::getSigned(Type::getInt32Ty(m_llvmContext), lit.getValue());
}

void CodeGen::visit(BinaryOp& binOp)
{
    logi << "Codegen::visit binary op";
    dispatch(binOp.getLhs());
    auto lhs = m_value;
    dispatch(binOp.getRhs());
    auto rhs = m_value;

    auto opType = binOp.getType();
    switch (opType)
    {
        case TokenKind::PLUS:
            m_value = m_irBuilder.CreateAdd(lhs, rhs);
            break;
        case TokenKind::MINUS:
            m_value = m_irBuilder.CreateSub(lhs, rhs);
            break;
        case TokenKind::TIMES:
            m_value = m_irBuilder.CreateMul(lhs, rhs);
            break;
        case TokenKind::DIV:
            m_value = m_irBuilder.CreateSDiv(lhs, rhs);
            break;
        default:
            loge << "unknown operator: " << opType;
    }
}
}
