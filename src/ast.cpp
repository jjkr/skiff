#include "ast.hpp"
#include "util/logger.hpp"

using std::move;
using std::unique_ptr;
using std::vector;

namespace sk
{
Module::Module(vector<unique_ptr<Expr>>&& expressions) : m_expressions(move(expressions))
{
}
BinaryOp::BinaryOp(Token::Kind kind, unique_ptr<Expr>&& lhs, unique_ptr<Expr>&& rhs)
    : m_kind(kind), m_lhs(move(lhs)), m_rhs(move(rhs))
{
}
Function::Function(Variable&& name, std::vector<Variable>&& parameters,
                   std::vector<std::unique_ptr<Expr>>&& expressions)
    : m_name(move(name)), m_parameters(move(parameters)), m_expressions(move(expressions))
{
}
}
