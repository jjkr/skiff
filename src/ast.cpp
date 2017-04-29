#include "ast.hpp"
#include "util/logger.hpp"

using std::move;
using std::make_unique;
using std::unique_ptr;
using std::vector;

namespace sk
{
Module::Module(string_view name) : m_name(name)
{
}

BinaryOp::BinaryOp(TokenType type, unique_ptr<Expr>&& lhs, unique_ptr<Expr>&& rhs)
    : m_type(type), m_lhs(move(lhs)), m_rhs(move(rhs))
{
    //children.emplace_back(lhs.release());
    //children.emplace_back(rhs.release());
}
Function::Function(Variable&& name, std::vector<unique_ptr<Variable>>&& parameters)
    : m_name(move(name)), m_parameters(move(parameters))
{
}
}
