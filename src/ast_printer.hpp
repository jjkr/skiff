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
    void visit(BinaryOp& binOp) override;

private:
    void indent();
    int m_depth = 0;
    std::ostream& m_out;
};
}