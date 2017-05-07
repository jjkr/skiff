#include "ast.hpp"
#include "util/logger.hpp"
#include <cassert>

using std::move;
using std::make_unique;
using std::ref;
using std::reference_wrapper;
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

TypeMatch::TypeMatch(std::unique_ptr<Identifier>&& id) : m_typeId(*id)
{
    addChild(move(id));
}

IdMatch::IdMatch(unique_ptr<Identifier>&& id, unique_ptr<TypeMatch>&& typeMatch)
    : m_id(*id), m_typeMatch(typeMatch.get())
{
    addChild(move(id));
    addChild(move(id));
}

TupleMatch::TupleMatch(vector<unique_ptr<Match>>&& matches)
{
    for (auto& m : matches)
    {
        m_matches.push_back(ref(*m));
        addChild(move(m));
    }
}

BinaryOp::BinaryOp(Token token, unique_ptr<Expr>&& lhs, unique_ptr<Expr>&& rhs)
    : m_token(token), m_lhs(*lhs), m_rhs(*rhs)
{
    addChild(move(lhs));
    addChild(move(rhs));
}

UnaryOp::UnaryOp(Token token, std::unique_ptr<Expr>&& arg) : m_token(token), m_arg(*arg)
{
    addChild(move(arg));
};

Function::Function(unique_ptr<Identifier>&& id, unique_ptr<TupleMatch>&& args)
    : m_id(id.get()), m_argMatch(args.get())
{
    assert(m_id);
    assert(m_argMatch);

    auto block = make_unique<Block>();
    m_block = block.get();

    addChild(move(id));
    addChild(move(args));
    addChild(move(block));
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
