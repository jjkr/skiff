#include "ast_visitor.hpp"
#include "ast.hpp"

namespace sk
{
void AstVisitor::dispatch(AstNode& node)
{
    node.accept(*this);
}
}