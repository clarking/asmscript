
#include "parser.h"
#include "lexer.h"

using namespace Lexer;

namespace Parser {
	
	using Statements = std::vector<std::unique_ptr<Statement>>;
	
	bool parserSuccess;
	std::unique_ptr<Token> *tokenPtr;
	std::unique_ptr<Token> *tokenEnd;
	
	bool EatToken(TokenTag tag);
	
	bool IsToken(TokenTag tag);
	
	TokenTag GetTag();
	
	CodePos GetPos();
	
	[[nodiscard]]  Error ParseProcedure(std::string &name, Statements &statements);
	
	[[nodiscard]]  Error ParseCondition(std::optional<Condition> &condition);
	
	[[nodiscard]]  Error ParseOperand(std::unique_ptr<Operand> &operand);
	
	[[nodiscard]]  Error ParseRegister(Register &reg);
	
	[[nodiscard]]  Error ParseStatement(Statements &statements);
	
	[[nodiscard]]  Error ParseAssignment(Statements &statements);
	
	[[nodiscard]]  Error ParseLoop(Statements &statements);
	
	[[nodiscard]]  Error ParseBranch(Statements &statements);
	
	[[nodiscard]]  Error ParseBreak(Statements &statements);
	
	[[nodiscard]]  Error ParseContinue(Statements &statements);
	
	[[nodiscard]]  Error ParseReturn(Statements &statements);
	
	[[nodiscard]]  Error ParseCall(Statements &statements);
	
	[[nodiscard]]  Error ParseStdout(Statements &statements);
	
	[[nodiscard]]  Error ParsePush(Statements &statements);
	
	[[nodiscard]]  Error ParsePop(Statements &statements);
	
	[[nodiscard]] Error Parse(std::vector<std::unique_ptr<Token>> &tokens, std::unordered_map<std::string, Statements> &procedures) {
		tokenPtr = &tokens.front();
		tokenEnd = &tokens.back();
		
		while (tokenPtr != tokenEnd) {
			std::string name;
			Statements statements;
			Error _error = (ParseProcedure(name, statements));
			if (_error)return _error;
			if (!parserSuccess) {
				return Error{"Unrecognized top-level declaration.", GetPos()};
			}
			else {
				auto it = procedures.find(name);
				if (it == procedures.end()) {
					procedures[name] = std::move(statements);
				}
				else {
					return Error{Format("Procedure \"%s\" redeclaration.", name.c_str()), GetPos()};
				}
			}
		}
		
		return Error::None;
	}
	
	bool EatToken(const TokenTag tag) {
		if ((*tokenPtr)->tag == tag) {
			tokenPtr += 1;
			return true;
		}
		
		return false;
	}
	
	bool IsToken(const TokenTag tag) {
		return (*tokenPtr)->tag == tag;
	}
	
	template<typename T>
	T *GetToken() {
		return static_cast<T *>(tokenPtr->get());
	}
	
	TokenTag GetTag() {
		return (*tokenPtr)->tag;
	}
	
	CodePos GetPos() {
		return (*tokenPtr)->pos;
	}
	
	[[nodiscard]]  Error ParseProcedure(std::string &name, Statements &statements) {
		if (!EatToken(TokenTag::KeyProc)) {
			parserSuccess = false;
			return Error::None;
		}
		
		if (!IsToken(TokenTag::Identifier)) {
			return Error{"Expected identifier in procedure declaration.", GetPos()};
		}
		name = GetToken<IdentifierToken>()->name;
		tokenPtr += 1;
		
		if (!EatToken(TokenTag::BraceOpen)) {
			return Error{"Expected { in procedure declaration.", GetPos()};
		}
		
		while (!EatToken(TokenTag::BraceClose)) {
			Error _error = (ParseStatement(statements));
			if (_error)return _error;
			if (!parserSuccess) {
				return Error{"Unrecognized statement.", GetPos()};
			}
		}
		
		parserSuccess = true;
		return Error::None;
	}
	
	/* NOTE For convenience, argument is std::optional, but this function assumes
	 * the condition is required at current position.
	 */
	[[nodiscard]]  Error ParseCondition(std::optional<Condition> &condition) {
		const CodePos pos = GetPos();
		
		std::unique_ptr<Operand> a;
		std::unique_ptr<Operand> b;
		
		Error _error = (ParseOperand(a));
		if (_error)return _error;
		
		const TokenTag tag = GetTag();
		
		Comparison comp;
		
		switch (tag) {
			case TokenTag::LessThan: comp = Comparison::LessThan;
				break;
			case TokenTag::LessEquals: comp = Comparison::LessEquals;
				break;
			case TokenTag::GreaterThan: comp = Comparison::GreaterThan;
				break;
			case TokenTag::GreaterEquals: comp = Comparison::GreaterEquals;
				break;
			case TokenTag::EqualsEquals: comp = Comparison::Equals;
				break;
			case TokenTag::NotEquals: comp = Comparison::NotEquals;
				break;
			default: return Error{"Unrecognized comparison operator, expected <, <=, >, >=, == or !=", GetPos()};
		}
		tokenPtr += 1;
		
		_error = (ParseOperand(b));
		if (_error)return _error;
		
		condition = Condition{std::move(a), std::move(b), comp, pos};
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseOperand(std::unique_ptr<Operand> &operand) {
		const CodePos pos = GetPos();
		switch (GetTag()) {
			case TokenTag::RegRax: operand = std::make_unique<RegisterOperand>(Register::rax, pos);
				break;
			case TokenTag::RegRbx: operand = std::make_unique<RegisterOperand>(Register::rbx, pos);
				break;
			case TokenTag::RegRcx: operand = std::make_unique<RegisterOperand>(Register::rcx, pos);
				break;
			case TokenTag::RegRdx: operand = std::make_unique<RegisterOperand>(Register::rdx, pos);
				break;
			case TokenTag::RegRsi: operand = std::make_unique<RegisterOperand>(Register::rsi, pos);
				break;
			case TokenTag::RegRdi: operand = std::make_unique<RegisterOperand>(Register::rdi, pos);
				break;
			case TokenTag::RegRbp: operand = std::make_unique<RegisterOperand>(Register::rbp, pos);
				break;
			case TokenTag::RegR8: operand = std::make_unique<RegisterOperand>(Register::r8, pos);
				break;
			case TokenTag::RegR9: operand = std::make_unique<RegisterOperand>(Register::r9, pos);
				break;
			case TokenTag::RegR10: operand = std::make_unique<RegisterOperand>(Register::r10, pos);
				break;
			case TokenTag::RegR11: operand = std::make_unique<RegisterOperand>(Register::r11, pos);
				break;
			case TokenTag::RegR12: operand = std::make_unique<RegisterOperand>(Register::r12, pos);
				break;
			case TokenTag::RegR13: operand = std::make_unique<RegisterOperand>(Register::r13, pos);
				break;
			case TokenTag::RegR14: operand = std::make_unique<RegisterOperand>(Register::r14, pos);
				break;
			case TokenTag::RegR15: operand = std::make_unique<RegisterOperand>(Register::r15, pos);
				break;
			case TokenTag::Number: operand = std::make_unique<ImmediateOperand>(GetToken<NumberToken>()->value, pos);
				break;
			case TokenTag::Minus: {
				tokenPtr += 1;
				if (!IsToken(TokenTag::Number)) {
					return Error{"Number expected after minus sign to form an immediate value.", pos};
				}
				operand = std::make_unique<ImmediateOperand>(-GetToken<NumberToken>()->value, pos);
				break;
			}
			default: return Error{"Unrecognized operand, expected register or immediate value.", pos};
		}
		tokenPtr += 1;
		
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseRegister(Register &reg) {
		switch (GetTag()) {
			case TokenTag::RegRax: reg = Register::rax;
				break;
			case TokenTag::RegRbx: reg = Register::rbx;
				break;
			case TokenTag::RegRcx: reg = Register::rcx;
				break;
			case TokenTag::RegRdx: reg = Register::rdx;
				break;
			case TokenTag::RegRsi: reg = Register::rsi;
				break;
			case TokenTag::RegRdi: reg = Register::rdi;
				break;
			case TokenTag::RegRbp: reg = Register::rbp;
				break;
			case TokenTag::RegR8: reg = Register::r8;
				break;
			case TokenTag::RegR9: reg = Register::r9;
				break;
			case TokenTag::RegR10: reg = Register::r10;
				break;
			case TokenTag::RegR11: reg = Register::r11;
				break;
			case TokenTag::RegR12: reg = Register::r12;
				break;
			case TokenTag::RegR13: reg = Register::r13;
				break;
			case TokenTag::RegR14: reg = Register::r14;
				break;
			case TokenTag::RegR15: reg = Register::r15;
				break;
			default: parserSuccess = false;
				return Error::None;
		}
		tokenPtr += 1;
		
		parserSuccess = true;
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseStatement(Statements &statements) {
		Error error = ParseAssignment(statements);
		if (error || parserSuccess) return error;
		
		error = ParseLoop(statements);
		if (error || parserSuccess) return error;
		
		error = ParseBranch(statements);
		if (error || parserSuccess) return error;
		
		error = ParseBreak(statements);
		if (error || parserSuccess) return error;
		
		error = ParseContinue(statements);
		if (error || parserSuccess) return error;
		
		error = ParseReturn(statements);
		if (error || parserSuccess) return error;
		
		error = ParseCall(statements);
		if (error || parserSuccess) return error;
		
		error = ParseStdout(statements);
		if (error || parserSuccess) return error;
		
		error = ParsePush(statements);
		if (error || parserSuccess) return error;
		
		error = ParsePop(statements);
		if (error || parserSuccess) return error;
		
		parserSuccess = false;
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseAssignment(Statements &statements) {
		const CodePos pos = GetPos();
		
		Register dest;
		Error error = ParseRegister(dest);
		if (!parserSuccess) return error;
		
		Operation op;
		bool isShorthand = true;
		
		switch (GetTag()) {
			case TokenTag::Equals: isShorthand = false;
				break;
			case TokenTag::PlusEquals: op = Operation::Add;
				break;
			case TokenTag::MinusEquals: op = Operation::Sub;
				break;
			case TokenTag::StarEquals: op = Operation::Mul;
				break;
			case TokenTag::SlashEquals: op = Operation::Div;
				break;
			case TokenTag::PercentEquals: op = Operation::Mod;
				break;
			case TokenTag::AmpersandEquals: op = Operation::And;
				break;
			case TokenTag::PipeEquals: op = Operation::Or;
				break;
			case TokenTag::CaretEquals: op = Operation::Xor;
				break;
			default: return Error{"Expected =, +=, -=, *=, /=, %=, &=, |= or ^=.", GetPos()};
		}
		tokenPtr += 1;
		
		std::unique_ptr<Operand> sourceA;
		Error _error = (ParseOperand(sourceA));
		if (_error)return _error;
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			_error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (EatToken(TokenTag::Semicolon)) {
			parserSuccess = true;
			if (isShorthand) {
				statements.emplace_back(std::make_unique<ShorthandStatement>(dest, op, std::move(sourceA), std::move(condition), pos));
			}
			else {
				statements.emplace_back(std::make_unique<AssignmentStatement>(dest, std::move(sourceA), std::move(condition), pos));
			}
			return Error::None;
		}
		else if (condition.has_value()) {
			return Error{"Expected ; after condition.", GetPos()};
		}
		else if (isShorthand) {
			return Error{"Expected ; or condition after source operand.", GetPos()};
		}
		
		switch (GetTag()) {
			case TokenTag::Plus: op = Operation::Add;
				break;
			case TokenTag::Minus: op = Operation::Sub;
				break;
			case TokenTag::Star: op = Operation::Mul;
				break;
			case TokenTag::Slash: op = Operation::Div;
				break;
			case TokenTag::Percent: op = Operation::Mod;
				break;
			case TokenTag::Ampersand: op = Operation::And;
				break;
			case TokenTag::Pipe: op = Operation::Or;
				break;
			case TokenTag::Caret: op = Operation::Xor;
				break;
			default: return Error{"Expected ;, +, -, *, /, %, &, | or ^.", GetPos()};
		}
		tokenPtr += 1;
		
		std::unique_ptr<Operand> sourceB;
		_error = (ParseOperand(sourceB));
		if (_error)return _error;
		
		if (EatToken(TokenTag::KeyIf)) {
			_error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<LonghandStatement>(dest, op, std::move(sourceA), std::move(sourceB), std::move(condition), pos));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseLoop(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyLoop)) {
			parserSuccess = false;
			return Error::None;
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::ParenOpen)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
			if (!EatToken(TokenTag::ParenClose)) {
				return Error{"Expected ) after loop condition.", GetPos()};
			}
		}
		
		if (!EatToken(TokenTag::BraceOpen)) {
			return Error{"Expected { in loop statement.", GetPos()};
		}
		
		Statements innerStatements;
		while (!EatToken(TokenTag::BraceClose)) {
			Error _error = (ParseStatement(innerStatements));
			if (_error)return _error;
			if (!parserSuccess) {
				return Error{"Unrecognized statement.", GetPos()};
			}
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<LoopStatement>(std::move(condition), std::move(innerStatements), pos));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseBranch(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyBranch)) {
			parserSuccess = false;
			return Error::None;
		}
		
		std::optional<Condition> condition;
		if (!EatToken(TokenTag::ParenOpen)) {
			return Error{"Expected ( after branch keyword.", GetPos()};
		}
		Error _error = (ParseCondition(condition));
		if (_error)return _error;
		
		if (!EatToken(TokenTag::ParenClose)) {
			return Error{"Expected ) after branch condition.", GetPos()};
		}
		
		if (!EatToken(TokenTag::BraceOpen)) {
			return Error{"Expected { in branch statement.", GetPos()};
		}
		
		Statements innerStatements;
		while (!EatToken(TokenTag::BraceClose)) {
			_error = (ParseStatement(innerStatements));
			if (_error)return _error;
			if (!parserSuccess) {
				return Error{"Unrecognized statement.", GetPos()};
			}
		}
		
		Statements elseBlock;
		if (EatToken(TokenTag::KeyElse)) {
			if (!EatToken(TokenTag::BraceOpen)) {
				return Error{"Expected { in else statement.", GetPos()};
			}
			
			while (!EatToken(TokenTag::BraceClose)) {
				_error = (ParseStatement(elseBlock));
				if (_error)return _error;
				if (!parserSuccess) {
					return Error{"Unrecognized statement.", GetPos()};
				}
			}
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<BranchStatement>(std::move(condition), std::move(innerStatements), std::move(elseBlock), pos));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseBreak(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyBreak)) {
			parserSuccess = false;
			return Error::None;
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<Statement>(StatementTag::Break, pos, std::move(condition)));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseContinue(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyContinue)) {
			parserSuccess = false;
			return Error::None;
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<Statement>(StatementTag::Continue, pos, std::move(condition)));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseReturn(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyReturn)) {
			parserSuccess = false;
			return Error::None;
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<Statement>(StatementTag::Return, pos, std::move(condition)));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseCall(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!IsToken(TokenTag::Identifier)) {
			parserSuccess = false;
			return Error::None;
		}
		
		const std::string &name = GetToken<IdentifierToken>()->name;
		tokenPtr += 1;
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<CallStatement>(name, std::move(condition), pos));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParseStdout(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::Shl)) {
			parserSuccess = false;
			return Error::None;
		}
		
		bool isText = false;
		const std::string *text;
		std::unique_ptr<Operand> source;
		
		if (IsToken(TokenTag::String)) {
			isText = true;
			text = &GetToken<StringToken>()->value;
			tokenPtr += 1;
		}
		else {
			Error _error = (ParseOperand(source));
			if (_error)return _error;
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			Error _error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		if (isText) {
			statements.emplace_back(std::make_unique<StdoutTextStatement>(*text, std::move(condition), pos));
		}
		else {
			statements.emplace_back(std::make_unique<StdoutStatement>(std::move(source), std::move(condition), pos));
		}
		return Error::None;
	}
	
	[[nodiscard]]  Error ParsePush(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyPush)) {
			parserSuccess = false;
			return Error::None;
		}
		
		Register reg;
		Error _error = (ParseRegister(reg));
		if (_error)return _error;
		if (!parserSuccess) {
			return Error{"Expected register.", GetPos()};
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			_error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<RegisterStatement>(StatementTag::Push, reg, std::move(condition), pos));
		return Error::None;
	}
	
	[[nodiscard]]  Error ParsePop(Statements &statements) {
		const CodePos pos = GetPos();
		
		if (!EatToken(TokenTag::KeyPop)) {
			parserSuccess = false;
			return Error::None;
		}
		
		Register reg;
		Error _error = (ParseRegister(reg));
		if (_error)return _error;
		if (!parserSuccess) {
			return Error{"Expected register.", GetPos()};
		}
		
		std::optional<Condition> condition;
		if (EatToken(TokenTag::KeyIf)) {
			_error = (ParseCondition(condition));
			if (_error)return _error;
		}
		
		if (!EatToken(TokenTag::Semicolon)) {
			return Error{"Expected ;.", GetPos()};
		}
		
		parserSuccess = true;
		statements.emplace_back(std::make_unique<RegisterStatement>(StatementTag::Pop, reg, std::move(condition), pos));
		return Error::None;
	}
}