#pragma once

namespace sk
{

class AstNode;
class Block;
class Module;
class Expr;
class Function;
class Variable;
class I32Literal;
class BinaryOp;

class AstVisitor
{
public:
    virtual void visit(Module& module) = 0;
    virtual void visit(Expr& expr) = 0;
    virtual void visit(Block& block) = 0;
    virtual void visit(Function& expr) = 0;
    virtual void visit(Variable& var) = 0;
    virtual void visit(I32Literal& literal) = 0;
    virtual void visit(BinaryOp& op) = 0;

    void dispatch(AstNode& node);
};
}