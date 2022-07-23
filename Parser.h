#pragma once

#include "CodePos.h"
#include "Error.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

enum class StatementTag {
	Assignment, // AssignmentStatement
	Shorthand,  // ShorthandStatement
	Longhand,   // LonghandStatement
	Loop,       // LoopStatement
	Branch,     // BranchStatement
	Break,      // Statement
	Continue,   // Statement
	Return,     // Statement
	Call,       // CallStatement
	Stdout,     // StdoutStatement
};

enum class OperandTag {
	Register,
	Immediate,
};

enum class Register {
	rax,
	rbx,
	rcx,
	rdx,
	rsi,
	rdi,
	rbp,
	r8,
	r9,
	r10,
	r11,
	r12,
	r13,
	r14,
	r15,
};

enum class Operation {
	Add,
	Sub,
	Mul,
	Div,
	Mod,
};

enum class Comparison {
	LessThan,
	LessEquals,
	GreaterThan,
	GreaterEquals,
	Equals,
	NotEquals,
};

struct Operand {
	OperandTag tag;
	CodePos pos;

	Operand(const OperandTag tag, const CodePos pos) : tag{tag}, pos{pos} {}
	virtual ~Operand() = 0;
};

inline Operand::~Operand() {}

struct RegisterOperand : public Operand {
	Register reg;

	RegisterOperand(const Register reg, const CodePos pos) : Operand{OperandTag::Register, pos}, reg{reg} {}
};

struct ImmediateOperand : public Operand {
	int64_t value;

	ImmediateOperand(const int64_t value, const CodePos pos) : Operand{OperandTag::Immediate, pos}, value{value} {}
};

struct Condition {
	std::unique_ptr<Operand> a;
	std::unique_ptr<Operand> b;
	Comparison comp;
	CodePos pos;

	Condition(std::unique_ptr<Operand> a, std::unique_ptr<Operand> b, const Comparison comp, const CodePos pos) : a{std::move(a)}, b{std::move(b)}, comp{comp}, pos{pos} {}
};

struct Statement {
	StatementTag tag;
	CodePos pos;
	std::optional<Condition> condition;

	Statement(const StatementTag tag, const CodePos pos, std::optional<Condition> condition) : tag{tag}, pos{pos}, condition{std::move(condition)} {}
	virtual ~Statement() = default;
};

struct AssignmentStatement : public Statement {
	Register dest;
	std::unique_ptr<Operand> source;

	AssignmentStatement(const Register dest, std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Assignment, pos, std::move(condition)}, dest{dest}, source{std::move(source)} {}
};

struct ShorthandStatement : public Statement {
	Register dest;
	Operation op;
	std::unique_ptr<Operand> source;

	ShorthandStatement(const Register dest, const Operation op, std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Shorthand, pos, std::move(condition)}, dest{dest}, op{op}, source{std::move(source)} {}
};

struct LonghandStatement : public Statement {
	Register dest;
	Operation op;
	std::unique_ptr<Operand> sourceA;
	std::unique_ptr<Operand> sourceB;

	LonghandStatement(const Register dest, const Operation op, std::unique_ptr<Operand> sourceA, std::unique_ptr<Operand> sourceB, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Longhand, pos, std::move(condition)}, dest{dest}, op{op}, sourceA{std::move(sourceA)}, sourceB{std::move(sourceB)} {}
};

struct LoopStatement : public Statement {
	std::vector<std::unique_ptr<Statement>> statements;

	LoopStatement(std::optional<Condition> condition, std::vector<std::unique_ptr<Statement>> statements, const CodePos pos) : Statement{StatementTag::Loop, pos, std::move(condition)}, statements{std::move(statements)} {}
};

struct BranchStatement : public Statement {
	std::vector<std::unique_ptr<Statement>> statements;
	std::vector<std::unique_ptr<Statement>> elseBlock;

	BranchStatement(std::optional<Condition> condition, std::vector<std::unique_ptr<Statement>> statements, std::vector<std::unique_ptr<Statement>> elseBlock, const CodePos pos) : Statement{StatementTag::Branch, pos, std::move(condition)}, statements{std::move(statements)}, elseBlock{std::move(elseBlock)} {}
};

struct StdoutStatement : public Statement {
	std::unique_ptr<Operand> source;

	StdoutStatement(std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Stdout, pos, std::move(condition)}, source{std::move(source)} {}
};

struct CallStatement : public Statement {
	std::string name;

	CallStatement(std::string name, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Call, pos, std::move(condition)}, name{std::move(name)} {}
};

struct Token;

[[nodiscard]] Error Parse(std::vector<std::unique_ptr<Token>>& tokens, std::unordered_map<std::string, std::vector<std::unique_ptr<Statement>>>& procedures);
