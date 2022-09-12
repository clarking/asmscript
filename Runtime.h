#pragma once

#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <cerrno>
#include <cstddef>
#include <cstdint>

#include "Common.h"
#include "Compiler.h"
#include <sys/mman.h>

namespace Runtime {

	struct Options {
		static bool flag_dumpTokens;
		static bool flag_dumpAst;
		static bool flag_dumpCode;
		static bool flag_noExec;
	};


	void Print(int64_t value);

	void Print(const char *text, size_t length);

	int RunFile(char *filepath);

	void PrintLexResults(std::string_view filePrefix, const std::vector<std::unique_ptr<Lexer::Token>> &tokens);

	void PrintParseResults(std::string_view filePrefix, const std::unordered_map<std::string, std::vector<std::unique_ptr<Parser::Statement>>> &procedures);

	void PrintCompileResults(const std::basic_string<unsigned char> &machineCode, size_t entry);

	void ExecuteCompileResults(const std::basic_string<unsigned char> &machineCode, size_t entry);

	void PrintStatements(std::string_view filePrefix, const std::vector<std::unique_ptr<Parser::Statement>> &statements, size_t level = 0);

	void PrintRegister(Compiler::Register reg);

	void PrintOperation(Operation op);

	void PrintCondition(const Condition &condition);

	void PrintOperand(const Operand &operand);
}