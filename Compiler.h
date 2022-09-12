#pragma once

#include "Types.h"
#include "Error.h"
#include "Parser.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstring>
#include <unordered_set>

namespace Compiler {

	using MachineCode = std::basic_string<unsigned char>;
	using Statements = std::vector<std::unique_ptr<Parser::Statement>>;

	[[nodiscard]] Runtime::Error Compile(
		std::unordered_map<std::string, std::vector<std::unique_ptr<Parser::Statement>>> &procedures,
		std::basic_string<unsigned char> &code, size_t &entry);

	static std::unordered_set<std::size_t> loopBreaks;
	static std::unordered_set<std::size_t> loopContinues;

	namespace Gen {

		void EmitRexW(bool r, bool b, MachineCode &code);

		void EmitRexB(MachineCode &code);

		void EmitModRM(uint8_t mod, uint8_t reg, uint8_t rm, MachineCode &code);

		void EmitSIB(uint8_t scale, uint8_t index, uint8_t base, MachineCode &code);

		void EmitImm8(int8_t value, MachineCode &code);

		void EmitImm32(int32_t value, MachineCode &code);

		void EmitImm64(int64_t value, MachineCode &code);

		void EmitPushAllRegs(MachineCode &code);

		void EmitPopAllRegs(MachineCode &code);

		void EmitMov(Register dest, Register source, MachineCode &code);

		void EmitMov(Register dest, int64_t value, MachineCode &code);

		void EmitLea(Register dest, size_t to, MachineCode &code);

		void EmitMovStack(Register dest, int64_t stackOffset, MachineCode &code);

		void EmitMovStack(int64_t stackOffset, Register source, MachineCode &code);

		void EmitAdd(Register dest, Register source, MachineCode &code);

		void EmitAdd(Register dest, int64_t value, MachineCode &code);

		void EmitSub(Register dest, Register source, MachineCode &code);

		void EmitSub(Register dest, int64_t value, MachineCode &code);

		void EmitAnd(Register dest, Register source, MachineCode &code);

		void EmitAnd(Register dest, int64_t value, MachineCode &code);

		void EmitOr(Register dest, Register source, MachineCode &code);

		void EmitOr(Register dest, int64_t value, MachineCode &code);

		void EmitXor(Register dest, Register source, MachineCode &code);

		void EmitXor(Register dest, int64_t value, MachineCode &code);

		void EmitImul(Register dest, Register source, MachineCode &code);

		void EmitImul(Register dest, Register source, int64_t value, MachineCode &code);

		void EmitIdiv(Register divisor, MachineCode &code);

		void EmitCmp(Register a, Register b, MachineCode &code, bool &inverted);

		void EmitCmp(Register a, int64_t b, MachineCode &code, bool &inverted);

		void EmitCmp(int64_t a, Register b, MachineCode &code, bool &inverted);

		void WriteJump(size_t from, size_t to, MachineCode &code);

		void WriteJump(size_t from, size_t to, Comparison comp, bool inverted, MachineCode &code);

		void EmitReturn(MachineCode &code);

		void EmitPush(Register reg, MachineCode &code);

		void EmitPop(Register reg, MachineCode &code);

		void EmitNop(size_t length, MachineCode &code);

		void EmitCall(Register reg, MachineCode &code);

		void WriteCall(size_t from, size_t to, MachineCode &code);

	}
};