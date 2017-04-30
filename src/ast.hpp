#pragma once
#include "ast_visitor.hpp"
#include "lexer.hpp"
#include "util/visitor.hpp"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace sk
{

class AstNode
{
public:
    virtual ~AstNode() {}
    virtual void accept(AstVisitor& visitor) = 0;

    std::vector<std::unique_ptr<AstNode>>& children() { return m_children; }
    const std::vector<std::unique_ptr<AstNode>>& children() const { return m_children; }

    void addChild(std::unique_ptr<AstNode>&& child) { m_children.push_back(std::move(child)); }

private:
    std::vector<std::unique_ptr<AstNode>> m_children;
};

class Block : public AstNode
{
public:
    Block(string_view name);
    virtual ~Block() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getName() const { return m_name; }
    std::vector<std::reference_wrapper<Expr>>& getExpressions() { return m_expressions; }

private:
    //std::map<std::string, Expr*> symbols;
    //std::vector<std::unique_ptr<Function>> functions;
    const string_view m_name;
    std::vector<std::reference_wrapper<Expr>> m_expressions;
};

class Module : public AstNode
{
public:
    Module(string_view name);
    virtual ~Module() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getName() const { return m_name; }
    Block& getMainBlock() { return m_mainBlock; }
    const Block& getMainBlock() const { return m_mainBlock; }

private:
    const string_view m_name;
    Block m_mainBlock;
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
    virtual ~Variable() {}
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
    BinaryOp(TokenKind kind, std::unique_ptr<Expr>&& lhs, std::unique_ptr<Expr>&& rhs);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    TokenKind getType() const { return m_kind; }
    Expr& getLhs() { return *m_lhs; }
    Expr& getRhs() { return *m_rhs; }

private:
    TokenKind m_kind;
    std::unique_ptr<Expr> m_lhs;
    std::unique_ptr<Expr> m_rhs;
};

class Function : public Expr
{
public:
    Function(string_view name, std::vector<string_view>&& parameters);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getName() const { return m_name; }
    std::vector<string_view>& getParameters() { return m_parameters; }
    Block& getBlock() { return m_block; }

private:
    const string_view m_name;
    std::vector<string_view> m_parameters;
    Block m_block;
};

class FunctionCall : public Expr
{
public:
    FunctionCall(Function& func);

private:
    Function& m_func;
    std::vector<std::unique_ptr<Expr>> m_arguments;
};

class LetExpr : public Expr
{
public:
    LetExpr(std::unique_ptr<Expr>&& id, std::unique_ptr<Expr>&& expr);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Expr& getIdentifier() { return *m_identifier; }
    Expr& getExpr() { return *m_expr; }

private:
    std::unique_ptr<Expr> m_identifier;
    std::unique_ptr<Expr> m_expr;
};

}
