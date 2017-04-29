#pragma once
#include "ast_visitor.hpp"
#include "lexer.hpp"
#include "util/visitor.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace sk
{

class AstNode
{
public:
    virtual ~AstNode() {}
    virtual void accept(AstVisitor& visitor) = 0;
};

class Module : public AstNode
{
public:
    Module(std::vector<std::unique_ptr<Expr>>&& expressions);
    virtual ~Module() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    std::vector<std::unique_ptr<Expr>>& getExpressions() { return m_expressions; }

private:
    std::vector<std::unique_ptr<Function>> m_functions;
    std::vector<std::unique_ptr<Expr>> m_expressions;
};

class Block : AstNode {
public:
    Block(std::vector<std::unique_ptr<Expr>>&& expressions);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }
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
    Function(Variable&& name, std::vector<std::unique_ptr<Variable>>&& parameters, std::vector<std::unique_ptr<Expr>>&& expressions);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Variable& getName() { return m_name; }
    std::vector<std::unique_ptr<Variable>>& getParameters() { return m_parameters; }
    std::vector<std::unique_ptr<Expr>>& getExpressions() { return m_expressions; }

private:
    Variable m_name;
    std::vector<std::unique_ptr<Variable>> m_parameters;
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
