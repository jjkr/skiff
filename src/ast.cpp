#include "ast.hpp"
#include "util/logger.hpp"

using std::move;
using std::make_unique;
using std::unique_ptr;
using std::vector;

namespace sk
{
Block::Block(string_view name) : m_name(name)
{
}

Module::Module(string_view name) : m_name(name), m_mainBlock("")
{
}

BinaryOp::BinaryOp(TokenKind kind, unique_ptr<Expr>&& lhs, unique_ptr<Expr>&& rhs)
    : m_kind(kind), m_lhs(*lhs), m_rhs(*rhs)
{
    addChild(move(lhs));
    addChild(move(rhs));
}
Function::Function(string_view name, std::vector<string_view>&& parameters)
    : m_name(name), m_parameters(move(parameters)), m_block("entry")
{
}

FunctionCall::FunctionCall(std::unique_ptr<Identifier>&& funcId,
                           std::vector<std::unique_ptr<Expr>>&& args)
    : m_funcId(*funcId), m_arguments()
{
    addChild(move(funcId));
    for (auto& arg : args)
    {
        m_arguments.push_back(*arg);
        addChild(move(arg));
    }
}

LetExpr::LetExpr(unique_ptr<Identifier>&& id, unique_ptr<Expr>&& expr)
    : m_identifier(*id), m_expr(*expr)
{
    addChild(move(id));
    addChild(move(expr));
}

IfExpr::IfExpr(std::unique_ptr<Expr>&& condition, std::unique_ptr<Block>&& thenBlock,
               std::unique_ptr<Block>&& elseBlock)
    : m_condition(condition.get()), m_thenBlock(thenBlock.get()), m_elseBlock(elseBlock.get())
{
    addChild(move(condition));
    addChild(move(thenBlock));
    addChild(move(elseBlock));
}
}
