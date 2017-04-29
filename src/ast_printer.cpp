#include "ast_printer.hpp"
#include "util/logger.hpp"

namespace sk
{
AstPrinter::AstPrinter(std::ostream& out) : m_out(out) {}

void AstPrinter::visit(Module& module)
{
    indent();
    m_out << "Module {\n";
    ++m_depth;
    for (const auto& expr: module.getExpressions())
    {
        expr->accept(*this);
    }
    --m_depth;
    m_out << "}\n";
}

void AstPrinter::visit(Expr& expr)
{
    indent();
    m_out << "Expr\n";
}

void AstPrinter::visit(Variable& var)
{
    indent();
    m_out << "Variable {" << var.getName() << "}\n";
}

void AstPrinter::visit(I32Literal& i32)
{
    indent();
    m_out << "I32Literal {" << i32.getValue() << "}\n";
}

void AstPrinter::visit(BinaryOp& binOp)
{
    indent();
    m_out << "BinaryOp " << binOp.getKind() << " {\n";
    ++m_depth;
    binOp.getLhs()->accept(*this);
    binOp.getRhs()-> accept(*this);
    --m_depth;
    indent();
    m_out << "}\n";
}

void AstPrinter::indent()
{
    for (auto i = 0; i < m_depth * 2; ++i)
    {
        m_out << " ";
    }
}
}
