#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "error.h"
#include "lexer.h"
#include "common.h"

using namespace Compiler;
using namespace Runtime;

namespace Parser {
	
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
		
		AssignmentStatement(const Register dest, std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Assignment, pos,
		                                                                                                                                             std::move(condition)},
		                                                                                                                                   dest{dest}, source{std::move(source)} {}
	};
	
	struct ShorthandStatement : public Statement {
		Register dest;
		Operation op;
		std::unique_ptr<Operand> source;
		
		ShorthandStatement(const Register dest, const Operation op, std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{
				StatementTag::Shorthand, pos, std::move(condition)}, dest{dest}, op{op}, source{std::move(source)} {}
	};
	
	struct LonghandStatement : public Statement {
		Register dest;
		Operation op;
		std::unique_ptr<Operand> sourceA;
		std::unique_ptr<Operand> sourceB;
		
		LonghandStatement(const Register dest, const Operation op, std::unique_ptr<Operand> sourceA, std::unique_ptr<Operand> sourceB, std::optional<Condition> condition,
				const CodePos pos) : Statement{StatementTag::Longhand, pos, std::move(condition)}, dest{dest}, op{op}, sourceA{std::move(sourceA)},
		                             sourceB{std::move(sourceB)} {}
	};
	
	struct LoopStatement : public Statement {
		std::vector<std::unique_ptr<Statement>> statements;
		
		LoopStatement(std::optional<Condition> condition, std::vector<std::unique_ptr<Statement>> statements, const CodePos pos) : Statement{StatementTag::Loop, pos,
		                                                                                                                                     std::move(condition)},
		                                                                                                                           statements{std::move(statements)} {}
	};
	
	struct BranchStatement : public Statement {
		std::vector<std::unique_ptr<Statement>> statements;
		std::vector<std::unique_ptr<Statement>> elseBlock;
		
		BranchStatement(std::optional<Condition> condition, std::vector<std::unique_ptr<Statement>> statements, std::vector<std::unique_ptr<Statement>> elseBlock,
				const CodePos pos) : Statement{StatementTag::Branch, pos, std::move(condition)}, statements{std::move(statements)}, elseBlock{std::move(elseBlock)} {}
	};
	
	struct StdoutStatement : public Statement {
		std::unique_ptr<Operand> source;
		
		StdoutStatement(std::unique_ptr<Operand> source, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Stdout, pos, std::move(condition)},
		                                                                                                          source{std::move(source)} {}
	};
	
	struct StdoutTextStatement : public Statement {
		std::string text;
		
		StdoutTextStatement(std::string text, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::StdoutText, pos, std::move(condition)},
		                                                                                               text{std::move(text)} {}
	};
	
	struct CallStatement : public Statement {
		std::string name;
		
		CallStatement(std::string name, std::optional<Condition> condition, const CodePos pos) : Statement{StatementTag::Call, pos, std::move(condition)}, name{std::move(name)} {}
	};
	
	struct RegisterStatement : public Statement {
		Register reg;
		
		RegisterStatement(const StatementTag tag, const Register reg, std::optional<Condition> condition, const CodePos pos) : Statement{tag, pos, std::move(condition)},
		                                                                                                                       reg{reg} {}
	};
	
	[[nodiscard]] Error Parse(std::vector<std::unique_ptr<Lexer::Token>> &tokens, std::unordered_map<std::string, std::vector<std::unique_ptr<Statement>>> &procedures);
}