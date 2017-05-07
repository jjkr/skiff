#include "code_gen.hpp"
#include "util/logger.hpp"
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using llvm::ConstantInt;
using llvm::Type;
using std::make_unique;
using std::move;
using std::ostringstream;
using std::runtime_error;
using std::string;
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

    // puts function
    vector<llvm::Type*> putsParameters = { llvm::Type::getInt8PtrTy(m_llvmContext) };
    auto putsType =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(m_llvmContext), move(putsParameters), false);
    auto putsFunc =
        llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", m_module.get());


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

    //auto returnBlock = llvm::BasicBlock::Create(m_llvmContext, "", llvmFunc);
    //m_irBuilder.SetInsertPoint(returnBlock);
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
    for (auto& m : func.getArgumentMatch().matches())
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
    for (auto& m : func.getArgumentMatch().matches())
    {
        auto& idMatch = dynamic_cast<const IdMatch&>(m.get());
        auto name = idMatch.getId().getName();
        arg->setName(llvm::StringRef(name.data(), name.size()));
        m_symbols[name] = &*arg;
    }

    m_functions[funcName] = llvmFunc;

    auto oldInsertBlock = m_irBuilder.GetInsertBlock();
    auto oldInsertPoint = m_irBuilder.GetInsertPoint();

    auto bb = llvm::BasicBlock::Create(m_llvmContext, "entry", llvmFunc);
    m_irBuilder.SetInsertPoint(bb);

    dispatch(func.getBlock());

    m_irBuilder.CreateRet(m_value);

    m_irBuilder.SetInsertPoint(oldInsertBlock, oldInsertPoint);

    m_value = ConstantInt::getSigned(Type::getInt32Ty(m_llvmContext), 0);
}

void CodeGen::visit(FunctionCall& call)
{
    logi << "Codegen::visit function call";
    auto funcName = call.getId().getName();
    auto llvmFunc = m_functions.find(funcName);
    if (llvmFunc == m_functions.end())
    {
        ostringstream ss;
        ss << "Could not resolve function: " << funcName;
        throw runtime_error(ss.str());
    }

    vector<llvm::Value*> llvmArgs;
    for (auto& arg : call.getArguments())
    {
        dispatch(arg);
        llvmArgs.push_back(m_value);
    }

    m_value = m_irBuilder.CreateCall(llvmFunc->second, llvmArgs);
}

void CodeGen::visit(Identifier& variable)
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

void CodeGen::visit(StringLiteral& str)
{
    logi << "Codegen::visit str literal";
    auto stringType =
        llvm::ArrayType::get(Type::getInt8Ty(m_llvmContext), str.getString().length() + 1);
    auto literal = str.getString();
    auto llvmConst =
        llvm::ConstantDataArray::getString(m_llvmContext, {literal.data(), literal.size()});
    m_value = new llvm::GlobalVariable(*m_module, stringType, true,
                                       llvm::GlobalValue::CommonLinkage, llvmConst);
}

void CodeGen::visit(BinaryOp& binOp)
{
    logi << "Codegen::visit binary op";
    dispatch(binOp.getLhs());
    auto lhs = m_value;
    dispatch(binOp.getRhs());
    auto rhs = m_value;

    auto op = binOp.getName();
    switch (op[0])
    {
        case '+':
            m_value = m_irBuilder.CreateAdd(lhs, rhs);
            break;
        case '-':
            m_value = m_irBuilder.CreateSub(lhs, rhs);
            break;
        case '*':
            m_value = m_irBuilder.CreateMul(lhs, rhs);
            break;
        case '/':
            m_value = m_irBuilder.CreateSDiv(lhs, rhs);
            break;
        default:
            loge << "unknown operator: " << op;
    }
}

void CodeGen::visit(IfExpr& expr)
{
    logi << "Codegen::visit if";
    dispatch(*expr.getCondition());

    auto* zeroConstant = ConstantInt::getSigned(Type::getInt32Ty(m_llvmContext), 0);
    auto* cond = m_irBuilder.CreateICmpNE(m_value, zeroConstant);
    auto* llvmFunc = m_irBuilder.GetInsertBlock()->getParent();
    auto* thenBlock = llvm::BasicBlock::Create(m_llvmContext, "then", llvmFunc);
    auto* elseBlock = llvm::BasicBlock::Create(m_llvmContext, "else");
    auto* mergeBlock = llvm::BasicBlock::Create(m_llvmContext, "ifcont");
    m_irBuilder.CreateCondBr(cond, thenBlock, elseBlock);

    m_irBuilder.SetInsertPoint(thenBlock);
    dispatch(*expr.getThenBlock());
    auto* thenValue = m_value;
    m_irBuilder.CreateBr(mergeBlock);
    thenBlock = m_irBuilder.GetInsertBlock();

    llvmFunc->getBasicBlockList().push_back(elseBlock);
    m_irBuilder.SetInsertPoint(elseBlock);
    dispatch(*expr.getElseBlock());
    auto* elseValue = m_value;
    m_irBuilder.CreateBr(mergeBlock);
    elseBlock = m_irBuilder.GetInsertBlock();

    llvmFunc->getBasicBlockList().push_back(mergeBlock);
    m_irBuilder.SetInsertPoint(mergeBlock);
    auto* phiNode = m_irBuilder.CreatePHI(Type::getInt32Ty(m_llvmContext), 2, "iftmp");

    phiNode->addIncoming(thenValue, thenBlock);
    phiNode->addIncoming(elseValue, elseBlock);

    m_value = phiNode;
}

void CodeGen::visit(Match& match)
{
    logi << "Codegen::visit match";
}

void CodeGen::visit(IdMatch& match)
{
    logi << "Codegen::visit id match";
}

void CodeGen::visit(TupleMatch& match)
{
    logi << "Codegen::visit template match";
}

void CodeGen::visit(TypeMatch& match)
{
    logi << "Codegen::visit type match";
}
}
