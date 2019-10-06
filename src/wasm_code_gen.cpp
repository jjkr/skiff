#include "wasm_code_gen.hpp"
#include "util/logger.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::ostream;

namespace sk
{
BinaryenModuleRelease::~BinaryenModuleRelease()
{
    BinaryenModuleDispose(m_module);
}

WasmCodeGen::WasmCodeGen() :
    m_module(BinaryenModuleCreate()),
    m_moduleRelease(m_module)
{
}

void WasmCodeGen::visit(Module& module)
{
    logi << "WasmCodegen::visit module";
    dispatch(module.getMainBlock());
}

void WasmCodeGen::visit(Block& block)
{
    logi << "WasmCodegen::visit block";
    for (auto& e : block.getExpressions())
    {
        dispatch(e);
    }
}

void WasmCodeGen::visit(LetExpr& expr)
{
    logi << "WasmCodegen::visit let";
}

void WasmCodeGen::visit(Expr& expr)
{
    logi << "WasmCodegen::visit expr";
}

void WasmCodeGen::visit(Function& func)
{
    logi << "WasmCodegen::visit function";
}

void WasmCodeGen::visit(FunctionCall& call)
{
}

void WasmCodeGen::visit(Identifier& variable)
{
}

void WasmCodeGen::visit(I32Literal& i32Literal)
{
}

void WasmCodeGen::visit(StringLiteral& str)
{
}

void WasmCodeGen::visit(BinaryOp& binOp)
{
}

void WasmCodeGen::visit(IfExpr& expr)
{
}

void WasmCodeGen::visit(Match& match)
{
}

void WasmCodeGen::visit(IdMatch& match)
{
}

void WasmCodeGen::visit(TupleMatch& match)
{
}

void WasmCodeGen::visit(TypeMatch& match)
{
}

void WasmCodeGen::printIr()
{
    BinaryenModulePrint(m_module);
}
}

