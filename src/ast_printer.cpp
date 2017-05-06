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
    dispatch(module.getMainBlock());
    --m_depth;
    m_out << "}\n";
}

void AstPrinter::visit(Block& block)
{
    //indent();
    //++m_depth;
    //m_out << "Block {\n";
    for (auto& expr: block.getExpressions())
    {
        dispatch(expr);
    }
    //--m_depth;
    //indent();
    //m_out << "}\n";
}

void AstPrinter::visit(LetExpr& expr)
{
    indent();
    m_out << "Let {\n";
    ++m_depth;
    dispatch(expr.getIdentifier());
    dispatch(expr.getExpr());
    --m_depth;
    indent();
    m_out << "}\n";
}

void AstPrinter::visit(Expr& expr)
{
    indent();
    m_out << "Expr\n";
}

void AstPrinter::visit(Function& func)
{
    indent();
    ++m_depth;
    m_out << "Function " << func.getName() << "(";
    auto first = true;
    for (auto& arg : func.getParameters()) {
        if (first)
        {
            first = false;
        }
        else
        {
            m_out << ", ";
        }
        m_out << arg;
    }
    m_out << ") {\n";
    dispatch(func.getBlock());
    --m_depth;
    indent();
    m_out << "}\n";
}

void AstPrinter::visit(FunctionCall& call)
{
    indent();
    ++m_depth;
    m_out << "FunctionCall " << call.getId().getName() << " args {\n";
    auto first = true;
    for (auto& arg : call.getArguments()) {
        if (first)
        {
            first = false;
        }
        else
        {
            m_out << ", ";
        }
        dispatch(arg);
    }
    --m_depth;
    indent();
    m_out << "}\n";
}

void AstPrinter::visit(Identifier& var)
{
    indent();
    m_out << "Identifier {" << var.getName() << "}\n";
}

void AstPrinter::visit(I32Literal& i32)
{
    indent();
    m_out << "I32Literal {" << i32.getValue() << "}\n";
}

void AstPrinter::visit(StringLiteral& srt)
{
    indent();
    m_out << "StringLiteral {" << srt.getString() << "}\n";
}

void AstPrinter::visit(BinaryOp& binOp)
{
    indent();
    m_out << "BinaryOp " << binOp.getType() << " {\n";
    ++m_depth;
    dispatch(binOp.getLhs());
    dispatch(binOp.getRhs());
    --m_depth;
    indent();
    m_out << "}\n";
}

void AstPrinter::visit(IfExpr& expr)
{
    indent();
    m_out << "IfExpr {\n";
    ++m_depth;
    dispatch(*expr.getCondition());
    dispatch(*expr.getThenBlock());
    dispatch(*expr.getElseBlock());
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
