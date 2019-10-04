#include "wasm_code_gen.hpp"
#include "util/logger.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sk
{
WasmCodeGen::WasmCodeGen(string_view sourceFile)
{
}

void WasmCodeGen::visit(Module& module)
{
}

void WasmCodeGen::visit(Block& block)
{
    logi << "WasmCodegen::visit block";
}

void WasmCodeGen::visit(LetExpr& expr)
{
}

void WasmCodeGen::visit(Expr& expr)
{
}

void WasmCodeGen::visit(Function& func)
{
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
}

