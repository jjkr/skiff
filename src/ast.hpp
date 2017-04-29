#pragma once
#include "lexer.hpp"
#include "util/visitor.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace sk
{

class Module;
class Expr;
class Variable;
class I32Literal;
class BinaryOp;
class AstVisitor
{
public:
    virtual void visit(Module& module) = 0;
    virtual void visit(Expr& expr) = 0;
    virtual void visit(Variable& var) = 0;
    virtual void visit(I32Literal& literal) = 0;
    virtual void visit(BinaryOp& op) = 0;
};

class AstNode : public Visitable<AstVisitor>
{
public:
    virtual ~AstNode() {}

};

class Module : public AstNode
{
public:
    Module(std::vector<std::unique_ptr<Expr>>&& expressions);
    virtual ~Module() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    std::vector<std::unique_ptr<Expr>>& getExpressions() { return m_expressions; }

private:
    std::vector<std::unique_ptr<Expr>> m_expressions;
};

class Expr : public AstNode
{
public:
    virtual ~Expr() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }
};

class Variable : public Expr
{
public:
    Variable(string_view name) : m_name(name) {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getName() const { return m_name; }
private:
    string_view m_name;
};

class I32Literal : public Expr
{
public:
    I32Literal(std::int32_t value) : m_value(value) {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    int32_t getValue() const { return m_value; }
private:
    const int32_t m_value;
};

class BinaryOp : public Expr
{
public:
    BinaryOp(TokenType kind, std::unique_ptr<Expr>&& lhs, std::unique_ptr<Expr>&& rhs);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    TokenType getType() const { return m_type; }
    std::unique_ptr<Expr>& getLhs() { return m_lhs; }
    std::unique_ptr<Expr>& getRhs() { return m_rhs; }

private:
    TokenType m_type;
    std::unique_ptr<Expr> m_lhs;
    std::unique_ptr<Expr> m_rhs;
};

class Function : public Expr
{
public:
    Function(Variable&& name, std::vector<Variable>&& parameters, std::vector<std::unique_ptr<Expr>>&& expressions);

    std::vector<Variable>& getParameters() { return m_parameters; }
    std::vector<std::unique_ptr<Expr>>& getExpressions() { return m_expressions; }

private:
    Variable m_name;
    std::vector<Variable> m_parameters;
    std::vector<std::unique_ptr<Expr>> m_expressions;
};

class FunctionCall : public Expr
{
public:
    FunctionCall(Function& func);

private:
    Function& m_func;
    std::vector<std::unique_ptr<Expr>> m_arguments;
};

}
