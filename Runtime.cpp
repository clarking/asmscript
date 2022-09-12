
#include "Runtime.h"

namespace Runtime {

	bool Options::flag_dumpTokens = false;
	bool Options::flag_dumpAst = false;
	bool Options::flag_dumpCode = false;
	bool Options::flag_noExec = false;

	void Print(int64_t value) {
		printf("%" PRId64, value);
	}

	void Print(const char *text, size_t length) {
		fwrite(text, 1, length, stdout);
	}

	int RunFile( char *filepath) {
		std::string code;
		if (!ReadFile(filepath, code)) {
			std::cerr << "Couldn't read file " << filepath << '\n';
			return 1;
		}

		std::vector<std::unique_ptr<Lexer::Token>> tokens;
		Error error = Lex(code.c_str(), tokens);
		if (error) {
			fprintf(stderr, "%s:%zu:%zu: Lexer error: %s\n", filepath, error.pos.line, error.pos.col, error.message.c_str());
			return 1;
		}

		if (Options::flag_dumpTokens) PrintLexResults(filepath, tokens);

		std::unordered_map<std::string, std::vector<std::unique_ptr<Parser::Statement>>> procedures;
		error = Parse(tokens, procedures);
		if (error) {
			fprintf(stderr, "%s:%zu:%zu: Parser error: %s\n", filepath, error.pos.line, error.pos.col, error.message.c_str());
			return 1;
		}

		if (Options::flag_dumpAst) PrintParseResults(filepath, procedures);

		std::basic_string<unsigned char> machineCode;
		size_t entry;
		error = Compiler::Compile(procedures, machineCode, entry);
		if (error) {
			fprintf(stderr, "%s:%zu:%zu: Compiler error: %s\n", filepath, error.pos.line, error.pos.col, error.message.c_str());
			return 1;
		}

		if (Options::flag_dumpCode) PrintCompileResults(machineCode, entry);
		if (!Options::flag_noExec) ExecuteCompileResults(machineCode, entry);

		return 0;
	}

	void PrintLexResults(const std::string_view filePrefix, const std::vector<std::unique_ptr<Lexer::Token>> &tokens) {
		for (const auto &token: tokens) {
			std::cout << filePrefix << ':' << token->pos.line << ':' << token->pos.col << ": ";

			switch (token->tag) {
				case Lexer::TokenTag::RegRax:
					std::cout << "RegRax";
					break;
				case Lexer::TokenTag::RegRbx:
					std::cout << "RegRbx";
					break;
				case Lexer::TokenTag::RegRcx:
					std::cout << "RegRcx";
					break;
				case Lexer::TokenTag::RegRdx:
					std::cout << "RegRdx";
					break;
				case Lexer::TokenTag::RegRsi:
					std::cout << "RegRsi";
					break;
				case Lexer::TokenTag::RegRdi:
					std::cout << "RegRdi";
					break;
				case Lexer::TokenTag::RegRbp:
					std::cout << "RegRbp";
					break;
				case Lexer::TokenTag::RegR8:
					std::cout << "RegR8";
					break;
				case Lexer::TokenTag::RegR9:
					std::cout << "RegR9";
					break;
				case Lexer::TokenTag::RegR10:
					std::cout << "RegR10";
					break;
				case Lexer::TokenTag::RegR11:
					std::cout << "RegR11";
					break;
				case Lexer::TokenTag::RegR12:
					std::cout << "RegR12";
					break;
				case Lexer::TokenTag::RegR13:
					std::cout << "RegR13";
					break;
				case Lexer::TokenTag::RegR14:
					std::cout << "RegR14";
					break;
				case Lexer::TokenTag::RegR15:
					std::cout << "RegR15";
					break;
				case Lexer::TokenTag::RegXmm0:
					std::cout << "RegXmm0";
					break;
				case Lexer::TokenTag::RegXmm1:
					std::cout << "RegXmm1";
					break;
				case Lexer::TokenTag::RegXmm2:
					std::cout << "RegXmm2";
					break;
				case Lexer::TokenTag::RegXmm3:
					std::cout << "RegXmm3";
					break;
				case Lexer::TokenTag::RegXmm4:
					std::cout << "RegXmm4";
					break;
				case Lexer::TokenTag::RegXmm5:
					std::cout << "RegXmm5";
					break;
				case Lexer::TokenTag::RegXmm6:
					std::cout << "RegXmm6";
					break;
				case Lexer::TokenTag::RegXmm7:
					std::cout << "RegXmm7";
					break;
				case Lexer::TokenTag::RegXmm8:
					std::cout << "RegXmm8";
					break;
				case Lexer::TokenTag::RegXmm9:
					std::cout << "RegXmm9";
					break;
				case Lexer::TokenTag::RegXmm10:
					std::cout << "RegXmm10";
					break;
				case Lexer::TokenTag::RegXmm11:
					std::cout << "RegXmm11";
					break;
				case Lexer::TokenTag::RegXmm12:
					std::cout << "RegXmm12";
					break;
				case Lexer::TokenTag::RegXmm13:
					std::cout << "RegXmm13";
					break;
				case Lexer::TokenTag::RegXmm14:
					std::cout << "RegXmm14";
					break;
				case Lexer::TokenTag::RegXmm15:
					std::cout << "RegXmm15";
					break;
				case Lexer::TokenTag::KeyBranch:
					std::cout << "KeyBranch";
					break;
				case Lexer::TokenTag::KeyBreak:
					std::cout << "KeyBreak";
					break;
				case Lexer::TokenTag::KeyContinue:
					std::cout << "KeyContinue";
					break;
				case Lexer::TokenTag::KeyElse:
					std::cout << "KeyElse";
					break;
				case Lexer::TokenTag::KeyIf:
					std::cout << "KeyIf";
					break;
				case Lexer::TokenTag::KeyLoop:
					std::cout << "KeyLoop";
					break;
				case Lexer::TokenTag::KeyMacro:
					std::cout << "KeyMacro";
					break;
				case Lexer::TokenTag::KeyPop:
					std::cout << "KeyPop";
					break;
				case Lexer::TokenTag::KeyProc:
					std::cout << "KeyProc";
					break;
				case Lexer::TokenTag::KeyPush:
					std::cout << "KeyPush";
					break;
				case Lexer::TokenTag::KeyReturn:
					std::cout << "KeyReturn";
					break;
				case Lexer::TokenTag::KeyVal:
					std::cout << "KeyVal";
					break;
				case Lexer::TokenTag::KeyVar:
					std::cout << "KeyVar";
					break;
				case Lexer::TokenTag::BracketOpen:
					std::cout << "BracketOpen";
					break;
				case Lexer::TokenTag::BracketClose:
					std::cout << "BracketClose";
					break;
				case Lexer::TokenTag::ParenOpen:
					std::cout << "ParenOpen";
					break;
				case Lexer::TokenTag::ParenClose:
					std::cout << "ParenClose";
					break;
				case Lexer::TokenTag::BraceOpen:
					std::cout << "BraceOpen";
					break;
				case Lexer::TokenTag::BraceClose:
					std::cout << "BraceClose";
					break;
				case Lexer::TokenTag::Plus:
					std::cout << "Plus";
					break;
				case Lexer::TokenTag::Minus:
					std::cout << "Minus";
					break;
				case Lexer::TokenTag::Star:
					std::cout << "Star";
					break;
				case Lexer::TokenTag::Slash:
					std::cout << "Slash";
					break;
				case Lexer::TokenTag::Percent:
					std::cout << "Percent";
					break;
				case Lexer::TokenTag::Ampersand:
					std::cout << "Ampersand";
					break;
				case Lexer::TokenTag::Pipe:
					std::cout << "Pipe";
					break;
				case Lexer::TokenTag::Caret:
					std::cout << "Caret";
					break;
				case Lexer::TokenTag::PlusEquals:
					std::cout << "PlusEquals";
					break;
				case Lexer::TokenTag::MinusEquals:
					std::cout << "MinusEquals";
					break;
				case Lexer::TokenTag::StarEquals:
					std::cout << "StarEquals";
					break;
				case Lexer::TokenTag::SlashEquals:
					std::cout << "SlashEquals";
					break;
				case Lexer::TokenTag::PercentEquals:
					std::cout << "PercentEquals";
					break;
				case Lexer::TokenTag::AmpersandEquals:
					std::cout << "AmpersandEquals";
					break;
				case Lexer::TokenTag::PipeEquals:
					std::cout << "PipeEquals";
					break;
				case Lexer::TokenTag::CaretEquals:
					std::cout << "CaretEquals";
					break;
				case Lexer::TokenTag::Equals:
					std::cout << "Equals";
					break;
				case Lexer::TokenTag::LessThan:
					std::cout << "LessThan";
					break;
				case Lexer::TokenTag::GreaterThan:
					std::cout << "GreaterThan";
					break;
				case Lexer::TokenTag::LessEquals:
					std::cout << "LessEquals";
					break;
				case Lexer::TokenTag::GreaterEquals:
					std::cout << "GreaterEquals";
					break;
				case Lexer::TokenTag::EqualsEquals:
					std::cout << "EqualsEquals";
					break;
				case Lexer::TokenTag::NotEquals:
					std::cout << "NotEquals";
					break;
				case Lexer::TokenTag::Hash:
					std::cout << "Hash";
					break;
				case Lexer::TokenTag::Shl:
					std::cout << "Shl";
					break;
				case Lexer::TokenTag::Shr:
					std::cout << "Shr";
					break;
				case Lexer::TokenTag::Comma:
					std::cout << "Comma";
					break;
				case Lexer::TokenTag::Semicolon:
					std::cout << "Semicolon";
					break;
				case Lexer::TokenTag::Number:
					std::cout << "Number " << dynamic_cast<Lexer::NumberToken *>(token.get())->value;
					break;
				case Lexer::TokenTag::Identifier:
					std::cout << "Identifier " << dynamic_cast<Lexer::IdentifierToken *>(token.get())->name;
					break;
				case Lexer::TokenTag::String:
					std::cout << "String " << dynamic_cast<Lexer::StringToken *>(token.get())->value;
					break;
				case Lexer::TokenTag::Eof:
					std::cout << "EOF";
					break;
			}

			std::cout << '\n';
		}
	}

	void PrintParseResults(const std::string_view filePrefix, const std::unordered_map<std::string, std::vector<std::unique_ptr<Parser::Statement>>> &procedures) {
		for (const auto &[name, statements]: procedures) {
			std::cout << "PROCEDURE " << name << "\n\n";

			PrintStatements(filePrefix, statements);

			std::cout << '\n';
		}
	}

	void PrintCompileResults(const std::basic_string<unsigned char> &machineCode, const size_t entry) {
		printf("Entry point is at 0x%016zX\n", entry);
		printf(
			"Runtime library function would be at:\n"
			"\t0x%016zX: void RtPrint(int64_t)\n"
			"\t0x%016zX: void RtPrint(const char*, size_t)\n",
			(size_t) (void (*)(int64_t)) (&Runtime::Print),
			(size_t) (void (*)(const char *, size_t)) (&Runtime::Print)
		);

		for (const unsigned char c: machineCode) {
			printf("%02X ", c);
		}

		puts("");
	}

	void ExecuteCompileResults(const std::basic_string<unsigned char> &machineCode, size_t entry) {
		const size_t len = machineCode.length();
		void *mem = mmap(nullptr, len, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (mem == MAP_FAILED) {
			fprintf(stderr, "Mapping memory failed with errno %d\n", errno);
			return;
		}
		memcpy(mem, machineCode.data(), len);
		mprotect(mem, len, PROT_EXEC | PROT_READ);

		char *entryPtr = static_cast<char *>(mem) + entry;
		void (*main)();
		memcpy(&main, &entryPtr, 8);
		main();

		munmap(mem, len);
	}

	void PrintStatements(const std::string_view filePrefix, const std::vector<std::unique_ptr<Parser::Statement>> &statements, const size_t level) {
		for (const auto &statement: statements) {
			for (size_t i = 0; i < level; ++i) std::cout << '\t';

			switch (statement->tag) {
				case StatementTag::Assignment: {
					auto stmt = dynamic_cast<Parser::AssignmentStatement *>(statement.get());
					std::cout << "Assignment ";
					PrintRegister(stmt->dest);
					std::cout << " = ";
					PrintOperand(*stmt->source);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Shorthand: {
					auto stmt = dynamic_cast<Parser::ShorthandStatement *>(statement.get());
					std::cout << "Shorthand ";
					PrintRegister(stmt->dest);
					std::cout << ' ';
					PrintOperation(stmt->op);
					std::cout << "= ";
					PrintOperand(*stmt->source);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Longhand: {
					auto stmt = dynamic_cast<Parser::LonghandStatement *>(statement.get());
					std::cout << "Longhand ";
					PrintRegister(stmt->dest);
					std::cout << " = ";
					PrintOperand(*stmt->sourceA);
					std::cout << ' ';
					PrintOperation(stmt->op);
					std::cout << ' ';
					PrintOperand(*stmt->sourceB);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Loop: {
					auto stmt = dynamic_cast<Parser::LoopStatement *>(statement.get());
					std::cout << "Loop";
					if (stmt->condition.has_value()) {
						std::cout << " (";
						PrintCondition(*stmt->condition);
						std::cout << ")";
					}
					std::cout << '\n';
					PrintStatements(filePrefix, stmt->statements, level + 1);
					continue;
				}
				case StatementTag::Branch: {
					auto stmt = dynamic_cast<Parser::BranchStatement *>(statement.get());
					std::cout << "Branch (";
					PrintCondition(*stmt->condition);
					std::cout << ")\n";
					PrintStatements(filePrefix, stmt->statements, level + 1);
					std::cout << "Else\n";
					PrintStatements(filePrefix, stmt->elseBlock, level + 1);
					continue;
				}
				case StatementTag::Break: {
					std::cout << "Break";
					if (statement->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*statement->condition);
					}
					break;
				}
				case StatementTag::Continue: {
					std::cout << "Continue";
					if (statement->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*statement->condition);
					}
					break;
				}
				case StatementTag::Return: {
					std::cout << "Return";
					if (statement->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*statement->condition);
					}
					break;
				}
				case StatementTag::Call: {
					auto stmt = dynamic_cast<Parser::CallStatement *>(statement.get());
					std::cout << "Call " << stmt->name;
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Stdout: {
					auto stmt = dynamic_cast<Parser::StdoutStatement *>(statement.get());
					std::cout << "Stdout ";
					PrintOperand(*stmt->source);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::StdoutText: {
					auto stmt = dynamic_cast<Parser::StdoutTextStatement *>(statement.get());
					std::cout << "StdoutText ";
					std::cout << stmt->text;
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Push: {
					auto stmt = dynamic_cast<Parser::RegisterStatement *>(statement.get());
					std::cout << "Push ";
					PrintRegister(stmt->reg);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
				case StatementTag::Pop: {
					auto stmt = dynamic_cast<Parser::RegisterStatement *>(statement.get());
					std::cout << "Pop ";
					PrintRegister(stmt->reg);
					if (stmt->condition.has_value()) {
						std::cout << " if ";
						PrintCondition(*stmt->condition);
					}
					break;
				}
			}
			std::cout << '\n';
		}
	}

	void PrintRegister(const Register reg) {
		switch (reg) {
			case Register::rax:
				std::cout << "rax";
				break;
			case Register::rbx:
				std::cout << "rbx";
				break;
			case Register::rcx:
				std::cout << "rcx";
				break;
			case Register::rdx:
				std::cout << "rdx";
				break;
			case Register::rsi:
				std::cout << "rsi";
				break;
			case Register::rdi:
				std::cout << "rdi";
				break;
			case Register::rbp:
				std::cout << "rbp";
				break;
			case Register::r8:
				std::cout << "r8";
				break;
			case Register::r9:
				std::cout << "r9";
				break;
			case Register::r10:
				std::cout << "r10";
				break;
			case Register::r11:
				std::cout << "r11";
				break;
			case Register::r12:
				std::cout << "r12";
				break;
			case Register::r13:
				std::cout << "r13";
				break;
			case Register::r14:
				std::cout << "r14";
				break;
			case Register::r15:
				std::cout << "r15";
				break;
		}
	}

	void PrintOperation(const Operation op) {
		switch (op) {
			case Operation::Add:
				std::cout << '+';
				break;
			case Operation::Sub:
				std::cout << '-';
				break;
			case Operation::Mul:
				std::cout << '*';
				break;
			case Operation::Div:
				std::cout << '/';
				break;
			case Operation::Mod:
				std::cout << '%';
				break;
			case Operation::And:
				std::cout << '&';
				break;
			case Operation::Or:
				std::cout << '|';
				break;
			case Operation::Xor:
				std::cout << '^';
				break;
		}
	}

	void PrintCondition(const Condition &condition) {
		PrintOperand(*condition.a);
		switch (condition.comp) {
			case Comparison::LessThan:
				std::cout << " < ";
				break;
			case Comparison::LessEquals:
				std::cout << " <= ";
				break;
			case Comparison::GreaterThan:
				std::cout << " > ";
				break;
			case Comparison::GreaterEquals:
				std::cout << " >= ";
				break;
			case Comparison::Equals:
				std::cout << " == ";
				break;
			case Comparison::NotEquals:
				std::cout << " != ";
				break;
		}
		PrintOperand(*condition.b);
	}

	void PrintOperand(const Operand &operand) {
		switch (operand.tag) {
			case OperandTag::Register:
				PrintRegister(dynamic_cast<const RegisterOperand &>(operand).reg);
				break;
			case OperandTag::Immediate:
				std::cout << dynamic_cast<const ImmediateOperand &>(operand).value;
				break;
		}
	}
}