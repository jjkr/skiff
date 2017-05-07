#include "ast.hpp"
#include <ostream>

namespace sk
{
class AstPrinter : public AstVisitor
{
public:
    AstPrinter(std::ostream& out);
    void visit(Module& module) override;
    void visit(Block& block) override;
    void visit(LetExpr& expr) override;
    void visit(Expr& expr) override;
    void visit(Function& function) override;
    void visit(FunctionCall& call) override;
    void visit(Identifier& variable) override;
    void visit(I32Literal& i32Literal) override;
    void visit(StringLiteral& i32Literal) override;
    void visit(BinaryOp& binOp) override;
    void visit(IfExpr& binOp) override;
    void visit(Match& match) override;
    void visit(IdMatch& match) override;
    void visit(TupleMatch& match) override;
    void visit(TypeMatch& match) override;

private:
    void indent();
    int m_depth = 0;
    std::ostream& m_out;
};
}