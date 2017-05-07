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
    Block(string_view name = "");
    virtual ~Block() {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getName() const { return m_name; }
    std::vector<std::reference_wrapper<Expr>>& getExpressions() { return m_expressions; }

private:
    //std::map<std::string, std::reference_wrapper<Expr>> m_variables;
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

class Identifier : public Expr
{
public:
    Identifier(string_view name) : m_name(name) {}
    virtual ~Identifier() {}
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
    BinaryOp(Token token, std::unique_ptr<Expr>&& lhs, std::unique_ptr<Expr>&& rhs);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Token getToken() const { return m_token; }
    string_view getName() const { return m_token.getStr(); }
    Expr& getLhs() { return m_lhs; }
    Expr& getRhs() { return m_rhs; }

private:
    const Token m_token;
    Expr& m_lhs;
    Expr& m_rhs;
};

class UnaryOp : public Expr
{
public:
    UnaryOp(Token token, std::unique_ptr<Expr>&& arg);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Token getToken() const { return m_token; }
    string_view getName() const { return m_token.getStr(); }
    Expr& getArgument() { return m_arg; }

private:
    const Token m_token;
    Expr& m_arg;
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
    FunctionCall(std::unique_ptr<Identifier>&& funcId, std::vector<std::unique_ptr<Expr>>&& args);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Identifier& getId() { return m_funcId; }
    std::vector<std::reference_wrapper<Expr>>& getArguments() { return m_arguments; }

private:
    Identifier& m_funcId;
    std::vector<std::reference_wrapper<Expr>> m_arguments;
};

class LetExpr : public Expr
{
public:
    LetExpr(std::unique_ptr<Identifier>&& id, std::unique_ptr<Expr>&& expr);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Identifier& getIdentifier() { return m_identifier; }
    Expr& getExpr() { return m_expr; }

private:
    Identifier& m_identifier;
    Expr& m_expr;
};

class IfExpr : public Expr
{
public:
    IfExpr(std::unique_ptr<Expr>&& condition, std::unique_ptr<Block>&& trueBlock,
           std::unique_ptr<Block>&& falseBlock);
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    Expr* getCondition() { return m_condition; }
    Block* getThenBlock() { return m_thenBlock; }
    Block* getElseBlock() { return m_elseBlock; }

private:
    Expr* m_condition;
    Block* m_thenBlock;
    Block* m_elseBlock;
};

class StringLiteral : public Expr
{
public:
    StringLiteral(string_view str) : m_str(str) {}
    void accept(AstVisitor& visitor) override { visitor.visit(*this); }

    string_view getString() const { return m_str; }
private:
    const string_view m_str;
};
}
