#pragma once
#include "RedyLexer.h"
#include <variant>
#include <map>
#pragma warning(disable:4146)
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

class TypeDeclAST;

class ExprAST {
public:
	TypeDeclAST* Type;

	virtual Value* CodeGen() = 0;
	TypeDeclAST* TypeCheck() {
		Type = OnTypeCheck();
		return Type;
	}

private:
	virtual TypeDeclAST* OnTypeCheck() = 0;
};

using ExprPtr = std::unique_ptr<ExprAST>;

class DoubleExpr : public ExprAST {
public:
	double Value;

	llvm::Value* CodeGen() override;

	DoubleExpr(double value) : Value(value) {}

private:
	TypeDeclAST* OnTypeCheck() override;
};

class BinOpExpr : public ExprAST {
public:
	ExprPtr LHS;
	TokenType Op;
	ExprPtr RHS;

	Value* CodeGen() override;

	BinOpExpr(ExprPtr lhs, TokenType op, ExprPtr rhs) : LHS(std::move(lhs)), Op(op), RHS(std::move(rhs)) {}

private:
	TypeDeclAST* OnTypeCheck() override;
};

class TypeAST {
public:
	std::string_view Name;

	TypeAST(std::string_view name) : Name(name) {}

	Type* CodeGen();
};

struct TypeASTComparer
{
	bool operator() (const TypeAST& lhs, const TypeAST& rhs) const
	{
		return lhs.Name < rhs.Name;
	}
};

class VariableAST {
public:
	TypeAST Type;
	std::string_view Name;
	ExprPtr DefaultValue;

	VariableAST(TypeAST type, std::string_view name, ExprPtr defaultValue = nullptr)
		: Type(type), Name(name), DefaultValue(std::move(defaultValue)) {}
};


class VariableExpr : public ExprAST {
public:
	VariableAST* Symbol;
	std::string_view Name;

	Value* CodeGen() override;

	VariableExpr(std::string_view name) : Name(name) {}

private:
	TypeDeclAST* OnTypeCheck() override;
};

class CallExpr : public ExprAST {
public:
	ExprPtr Callee;
	std::vector<ExprPtr> Params;

	Value* CodeGen() override;

	CallExpr(ExprPtr callee, std::vector<ExprPtr> params)
		: Callee(std::move(callee)), Params(std::move(params)) {}

private:
	TypeDeclAST* OnTypeCheck() override;
};

class UnaryExpr : public ExprAST {
public:
	TokenType Op;
	ExprPtr Expr;

	Value* CodeGen() override;

	UnaryExpr(TokenType op, ExprPtr expr) : Op(op), Expr(std::move(expr)) {}

private:
	TypeDeclAST* OnTypeCheck() override;
};


enum class VisibilityAST {
	Public,
	Private
};

class ProtoAST {
public:
	VisibilityAST Visibility;
	TypeAST Type;
	std::string_view Name;
	std::vector<VariableAST> Params;

	ProtoAST(VisibilityAST visibility, TypeAST type, std::string_view name, std::vector<VariableAST> params)
		: Visibility(visibility), Type(type), Name(name), Params(std::move(params)) {}

	Function* CodeGen();

};

class FuncAST {
public:
	ProtoAST Proto;
	ExprPtr Body;

	FuncAST(ProtoAST proto, ExprPtr body) : Proto(std::move(proto)), Body(std::move(body)) {}

	void TypeCheck();
	Function* CodeGen();
};

class StatementAST {
	virtual void TypeCheck() = 0;
	virtual void CodeGen() = 0;
};

class VariableDeclaration : public StatementAST {
public:
	VariableAST Variable;

	void TypeCheck() override;
	void CodeGen() override;

	VariableDeclaration(VariableAST variable) : Variable(std::move(variable)) {}
};

// TODO: add other visiblites like pub(get) and pub(set)
class FieldAST {
public:
	VisibilityAST Visibility;
	VariableAST Variable;

	FieldAST(VisibilityAST visibility, VariableAST variable)
		: Visibility(visibility), Variable(std::move(variable)) {}
};

class TypeDeclAST {
public:
	VisibilityAST Visibility;
	std::string_view Name;
	std::vector<FuncAST> Methods;

	TypeDeclAST(VisibilityAST visibility, std::string_view name, std::vector<FuncAST> methods)
		: Visibility(visibility), Name(name), Methods(std::move(methods)) {}
};

class StructAST {
public:
	TypeDeclAST TypeDecl;
	std::vector<FieldAST> Fields;
	
	void TypeCheck();
	void CodeGen();

	StructAST(std::vector<FieldAST> fields, TypeDeclAST typeDecl)
		: Fields(std::move(fields)), TypeDecl(std::move(typeDecl)) {}
};

class ModuleAST {
public:
	std::vector<StructAST> Structs;

	void TypeCheck();
	void CodeGen();

	ModuleAST(std::vector<StructAST> structs) : Structs(std::move(structs)) {}
};