#include "common.h"
#include "compiler.h"
#include "parser.h"
#include "runtime.h"

namespace Compiler {
	
	namespace Gen {
		
		// --- EMIT HELPERS
		
		void EmitRexW(const bool r, const bool b, MachineCode &code) {
			code.push_back(0x48 | (r << 2) | (b << 0));
		}
		
		void EmitRexB(MachineCode &code) {
			code.push_back(0x41);
		}
		
		void EmitModRM(const uint8_t mod, const uint8_t reg, const uint8_t rm, MachineCode &code) {
			const uint8_t byte = (mod << 6) | (reg << 3) | (rm << 0);
			code.push_back(byte);
		}
		
		void EmitSIB(const uint8_t scale, const uint8_t index, const uint8_t base, MachineCode &code) {
			const uint8_t byte = (scale << 6) | (index << 3) | (base << 0);
			code.push_back(byte);
		}
		
		void EmitImm8(const int8_t value, MachineCode &code) {
			code.push_back(static_cast<unsigned char>(value));
		}
		
		void EmitImm32(const int32_t value, MachineCode &code) {
			unsigned char val[4];
			memcpy(val, &value, 4);
			code.append(val, 4);
		}
		
		void EmitImm64(const int64_t value, MachineCode &code) {
			unsigned char val[8];
			memcpy(val, &value, 8);
			code.append(val, 8);
		}
		
		void EmitPushAllRegs(MachineCode &code) {
			EmitPush(Register::rax, code);
			EmitPush(Register::rbx, code);
			EmitPush(Register::rcx, code);
			EmitPush(Register::rdx, code);
			EmitPush(Register::rsi, code);
			EmitPush(Register::rdi, code);
			EmitPush(Register::rbp, code);
			EmitPush(Register::r8, code);
			EmitPush(Register::r9, code);
			EmitPush(Register::r10, code);
			EmitPush(Register::r11, code);
			EmitPush(Register::r12, code);
			EmitPush(Register::r13, code);
			EmitPush(Register::r14, code);
			EmitPush(Register::r15, code);
		}
		
		void EmitPopAllRegs(MachineCode &code) {
			EmitPop(Register::r15, code);
			EmitPop(Register::r14, code);
			EmitPop(Register::r13, code);
			EmitPop(Register::r12, code);
			EmitPop(Register::r11, code);
			EmitPop(Register::r10, code);
			EmitPop(Register::r9, code);
			EmitPop(Register::r8, code);
			EmitPop(Register::rbp, code);
			EmitPop(Register::rdi, code);
			EmitPop(Register::rsi, code);
			EmitPop(Register::rdx, code);
			EmitPop(Register::rcx, code);
			EmitPop(Register::rbx, code);
			EmitPop(Register::rax, code);
		}
		
		// --- EMIT FULL INSTRUCTION
		
		void EmitMov(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x89);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitMov(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			EmitRexW(false, destval & 0x08, code);
			code.push_back(0xB8 | (destval & 0x07));
			EmitImm64(value, code);
		}
		
		void EmitLea(const Register dest, const size_t to, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			size_t from = code.length() + 7;
			int32_t diff = static_cast<int32_t>(to) - static_cast<int32_t>(from);
			
			EmitRexW(destval & 0x08, false, code);
			code.push_back(0x8D);
			EmitModRM(0b00, destval & 0x07, 5, code);
			EmitImm32(diff, code);
		}
		
		void EmitMovStack(const Register dest, const int64_t stackOffset, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			int64_t disp = stackOffset * 8;
			
			EmitRexW(destval & 0x08, false, code);
			code.push_back(0x8B);
			
			if (disp >= INT64_C(-128) && disp <= INT64_C(127)) {
				EmitModRM(0b01, destval & 0x07, 0b100, code);
				EmitSIB(0b00, 0b100, 0x04, code);
				EmitImm8(static_cast<int8_t>(disp), code);
			}
			else {
				EmitModRM(0b10, destval & 0x07, 0b100, code);
				EmitSIB(0b00, 0b100, 0x04, code);
				EmitImm32(static_cast<int32_t>(disp), code);
			}
		}
		
		void EmitMovStack(const int64_t stackOffset, const Register source, MachineCode &code) {
			const auto srcval = static_cast<uint8_t>(source);
			int64_t disp = stackOffset * 8;
			
			EmitRexW(srcval & 0x08, false, code);
			code.push_back(0x89);
			
			if (disp >= INT64_C(-128) && disp <= INT64_C(127)) {
				EmitModRM(0b01, srcval & 0x07, 0b100, code);
				EmitSIB(0b00, 0b100, 0x04, code);
				EmitImm8(static_cast<int8_t>(disp), code);
			}
			else {
				EmitModRM(0b10, srcval & 0x07, 0b100, code);
				EmitSIB(0b00, 0b100, 0x04, code);
				EmitImm32(static_cast<int32_t>(disp), code);
			}
		}
		
		void EmitAdd(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x01);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitAdd(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 0, destval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 0, destval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				Register tmp = dest != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, value, code);
				EmitAdd(dest, tmp, code);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitSub(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x29);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitSub(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 5, destval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 5, destval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				Register tmp = dest != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, value, code);
				EmitSub(dest, tmp, code);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitAnd(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x21);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitAnd(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 4, destval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 4, destval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				Register tmp = dest != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, value, code);
				EmitAnd(dest, tmp, code);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitOr(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x09);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitOr(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 1, destval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 1, destval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				Register tmp = dest != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, value, code);
				EmitOr(dest, tmp, code);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitXor(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(srcval & 0x08, destval & 0x08, code);
			code.push_back(0x31);
			EmitModRM(0b11, srcval & 0x07, destval & 0x07, code);
		}
		
		void EmitXor(const Register dest, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 6, destval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(false, destval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 6, destval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				Register tmp = dest != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, value, code);
				EmitXor(dest, tmp, code);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitImul(const Register dest, const Register source, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			EmitRexW(destval & 0x08, srcval & 0x08, code);
			code.push_back(0x0F);
			code.push_back(0xAF);
			EmitModRM(0b11, destval & 0x07, srcval & 0x07, code);
		}
		
		void EmitImul(const Register dest, const Register source, const int64_t value, MachineCode &code) {
			const auto destval = static_cast<uint8_t>(dest);
			const auto srcval = static_cast<uint8_t>(source);
			
			if (value >= INT64_C(-128) && value <= INT64_C(127)) {
				EmitRexW(destval & 0x08, srcval & 0x08, code);
				code.push_back(0x6B);
				EmitModRM(0b11, destval & 0x07, srcval & 0x07, code);
				EmitImm8(static_cast<int8_t>(value), code);
			}
			else if (value >= INT64_C(-2147483648) && value <= INT64_C(2147483647)) {
				EmitRexW(destval & 0x08, srcval & 0x08, code);
				code.push_back(0x69);
				EmitModRM(0b11, destval & 0x07, srcval & 0x07, code);
				EmitImm32(static_cast<int32_t>(value), code);
			}
			else {
				EmitMov(dest, value, code);
				EmitImul(dest, source, code);
			}
		}
		
		void EmitIdiv(const Register divisor, MachineCode &code) {
			const auto divisorval = static_cast<uint8_t>(divisor);
			EmitRexW(false, divisorval & 0x08, code);
			code.push_back(0xF7);
			EmitModRM(0b11, 7, divisorval & 0x07, code);
		}
		
		void EmitCmp(const Register a, const Register b, MachineCode &code, bool &inverted) {
			const auto aval = static_cast<uint8_t>(a);
			const auto bval = static_cast<uint8_t>(b);
			
			EmitRexW(bval & 0x08, aval & 0x08, code);
			code.push_back(0x39);
			EmitModRM(0b11, bval & 0x07, aval & 0x07, code);
			
			(void) inverted;
		}
		
		void EmitCmp(const Register a, const int64_t b, MachineCode &code, bool &inverted) {
			const auto aval = static_cast<uint8_t>(a);
			
			if (b >= INT64_C(-128) && b <= INT64_C(127)) {
				EmitRexW(false, aval & 0x08, code);
				code.push_back(0x83);
				EmitModRM(0b11, 7, aval & 0x07, code);
				EmitImm8(static_cast<int8_t>(b), code);
			}
			else if (b >= INT64_C(-2147483648) && b <= INT64_C(2147483647)) {
				EmitRexW(false, aval & 0x08, code);
				code.push_back(0x81);
				EmitModRM(0b11, 7, aval & 0x07, code);
				EmitImm32(static_cast<int32_t>(b), code);
			}
			else {
				Register tmp = a != Register::rax ? Register::rax : Register::rbx;
				EmitMovStack(-1, tmp, code);
				EmitMov(tmp, b, code);
				EmitCmp(a, tmp, code, inverted);
				EmitMovStack(tmp, -1, code);
			}
		}
		
		void EmitCmp(const int64_t a, const Register b, MachineCode &code, bool &inverted) {
			inverted = !inverted;
			EmitCmp(b, a, code, inverted);
		}
		
		void WriteJump(const size_t from, const size_t to, MachineCode &code) {
			int32_t diff = static_cast<int32_t>(to) - (static_cast<int32_t>(from) + 5);
			code[from] = 0xE9;
			memcpy(&code[from + 1], &diff, 4);
		}
		
		void WriteJump(const size_t from, const size_t to, Comparison comp, const bool inverted, MachineCode &code) {
			if (inverted) {
				switch (comp) {
					case Comparison::LessThan: comp = Comparison::GreaterEquals;
						break;
					case Comparison::LessEquals: comp = Comparison::GreaterThan;
						break;
					case Comparison::GreaterThan: comp = Comparison::LessEquals;
						break;
					case Comparison::GreaterEquals: comp = Comparison::LessThan;
						break;
					case Comparison::Equals: comp = Comparison::NotEquals;
						break;
					case Comparison::NotEquals: comp = Comparison::Equals;
						break;
				}
			}
			
			int32_t diff = static_cast<int32_t>(to) - (static_cast<int32_t>(from) + 6);
			const uint8_t opcode = 0x10 + static_cast<uint8_t>(comp);
			
			code[from] = 0x0F;
			code[from + 1] = opcode;
			memcpy(&code[from + 2], &diff, 4);
		}
		
		void EmitReturn(MachineCode &code) {
			code.push_back(0xC3);
		}
		
		void EmitPush(const Register reg, MachineCode &code) {
			const auto regval = static_cast<uint8_t>(reg);
			
			if (regval & 0x08) EmitRexB(code);
			code.push_back(0x50 | (regval & 0x07));
		}
		
		void EmitPop(const Register reg, MachineCode &code) {
			const auto regval = static_cast<uint8_t>(reg);
			
			if (regval & 0x08) EmitRexB(code);
			code.push_back(0x58 | (regval & 0x07));
		}
		
		void EmitNop(const size_t length, MachineCode &code) {
			for (size_t i = 0; i < length; ++i) code.push_back(0x90);
		}
		
		void EmitCall(const Register reg, MachineCode &code) {
			const auto regval = static_cast<uint8_t>(reg);
			
			if (regval & 0x08) EmitRexB(code);
			code.push_back(0xFF);
			EmitModRM(0b11, 2, regval & 0x07, code);
		}
		
		void WriteCall(const size_t from, const size_t to, MachineCode &code) {
			int32_t diff = static_cast<int32_t>(to) - (static_cast<int32_t>(from) + 5);
			
			code[from] = 0xE8;
			memcpy(&code[from + 1], &diff, 4);
		}
	}
	
	[[nodiscard]]  Error CompileProcedure(const Statements &statements, MachineCode &code, std::unordered_map<size_t, std::string> &callTable);
	
	[[nodiscard]]  Error CompileStatement(const Parser::Statement &statement, MachineCode &code, std::unordered_map<size_t, std::string> &callTable);
	
	[[nodiscard]]  Error CompileCondition(const Condition &condition, MachineCode &code, bool &inverted);
	
	void CompileStartProcedure(MachineCode &code, std::unordered_map<size_t, std::string> &callTable);
	
	
	[[nodiscard]] Error Compile(std::unordered_map<std::string, Statements> &procedures, MachineCode &code, size_t &entry) {
		std::unordered_map<std::string, size_t> procedureMap;
		std::unordered_map<size_t, std::string> callTable;
		
		for (const auto &[name, statements]: procedures) {
			const size_t ptr = code.length();
			procedureMap[name] = ptr;
			
			Error _error = (CompileProcedure(statements, code, callTable));
			if (_error)return _error;
		}
		
		entry = code.length();
		CompileStartProcedure(code, callTable);
		
		for (const auto &[ptr, name]: callTable) {
			auto it = procedureMap.find(name);
			if (it == procedureMap.end()) {
				return Error{Format("Calling procedure \"%s\", which doesn't exist.", name.c_str()), CodePos{0, 0}}; // TODO Actual position in code
			}
			else {
				const size_t dest = it->second;
				Gen::WriteCall(ptr, dest, code);
			}
		}
		
		return Error::None;
	}
	
	[[nodiscard]]  Error CompileProcedure(const Statements &statements, MachineCode &code, std::unordered_map<size_t, std::string> &callTable) {
		for (const auto &statement: statements) {
			Error _error = (CompileStatement(*statement, code, callTable));
			if (_error)return _error;
		}
		
		if (!loopBreaks.empty() || !loopContinues.empty()) {
			return Error{"Break or continue statements in a procedure outside a loop.", CodePos{0, 0}};
		}
		
		Gen::EmitReturn(code);
		return Error::None;
	}
	
	[[nodiscard]]  Error CompileStatement(const Parser::Statement &statement, MachineCode &code, std::unordered_map<size_t, std::string> &callTable) {
		const size_t startPtr = code.length();
		
		bool conditionalJumpInverted = true;
		size_t conditionalJumpPtr = 0;
		if (statement.condition.has_value()) {
			Error _error = (CompileCondition(*statement.condition, code, conditionalJumpInverted));
			if (_error)return _error;
			conditionalJumpPtr = code.length();
			Gen::EmitNop(6, code);
		}
		
		switch (statement.tag) {
			case StatementTag::Assignment: {
				const auto &stmt = dynamic_cast<const Parser::AssignmentStatement &>(statement);
				switch (stmt.source->tag) {
					case OperandTag::Register: Gen::EmitMov(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
						break;
					case OperandTag::Immediate: Gen::EmitMov(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
						break;
					default: return Error{"Unsopported source argument type.", statement.pos};
				}
				break;
			}
			case StatementTag::Shorthand: {
				const auto &stmt = dynamic_cast<const Parser::ShorthandStatement &>(statement);
				switch (stmt.op) {
					case Operation::Add:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitAdd(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitAdd(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Sub:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitSub(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitSub(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Mul:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitImul(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitImul(stmt.dest, stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Div:
						switch (stmt.source->tag) {
							case OperandTag::Register: {
								Gen::EmitMovStack(-1, Register::rax, code);
								Gen::EmitMovStack(-2, Register::rdx, code);
								Gen::EmitMovStack(-3, Register::rbx, code);
								
								Gen::EmitMovStack(-4, stmt.dest, code);
								Gen::EmitMovStack(-5, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								
								Gen::EmitMov(Register::rdx, 0, code);
								Gen::EmitMovStack(Register::rax, -4, code);
								Gen::EmitMovStack(Register::rbx, -5, code);
								
								Gen::EmitIdiv(Register::rbx, code);
								Gen::EmitMovStack(-4, Register::rax, code);
								
								Gen::EmitMovStack(Register::rax, -1, code);
								Gen::EmitMovStack(Register::rdx, -2, code);
								Gen::EmitMovStack(Register::rbx, -3, code);
								
								Gen::EmitMovStack(stmt.dest, -4, code);
								break;
							}
							case OperandTag::Immediate: {
								Gen::EmitMovStack(-1, Register::rax, code);
								Gen::EmitMovStack(-2, Register::rdx, code);
								Gen::EmitMovStack(-3, Register::rbx, code);
								
								Gen::EmitMovStack(-4, stmt.dest, code);
								
								Gen::EmitMov(Register::rdx, 0, code);
								Gen::EmitMovStack(Register::rax, -4, code);
								Gen::EmitMovStack(Register::rbx, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								
								Gen::EmitIdiv(Register::rbx, code);
								Gen::EmitMovStack(-4, Register::rax, code);
								
								Gen::EmitMovStack(Register::rax, -1, code);
								Gen::EmitMovStack(Register::rdx, -2, code);
								Gen::EmitMovStack(Register::rbx, -3, code);
								
								Gen::EmitMovStack(stmt.dest, -4, code);
								break;
							}
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Mod:
						switch (stmt.source->tag) {
							case OperandTag::Register: {
								Gen::EmitMovStack(-1, Register::rax, code);
								Gen::EmitMovStack(-2, Register::rdx, code);
								Gen::EmitMovStack(-3, Register::rbx, code);
								
								Gen::EmitMovStack(-4, stmt.dest, code);
								Gen::EmitMovStack(-5, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								
								Gen::EmitMov(Register::rdx, 0, code);
								Gen::EmitMovStack(Register::rax, -4, code);
								Gen::EmitMovStack(Register::rbx, -5, code);
								
								Gen::EmitIdiv(Register::rbx, code);
								Gen::EmitMovStack(-4, Register::rdx, code);
								
								Gen::EmitMovStack(Register::rax, -1, code);
								Gen::EmitMovStack(Register::rdx, -2, code);
								Gen::EmitMovStack(Register::rbx, -3, code);
								
								Gen::EmitMovStack(stmt.dest, -4, code);
								break;
							}
							case OperandTag::Immediate: {
								Gen::EmitMovStack(-1, Register::rax, code);
								Gen::EmitMovStack(-2, Register::rdx, code);
								Gen::EmitMovStack(-3, Register::rbx, code);
								
								Gen::EmitMovStack(-4, stmt.dest, code);
								
								Gen::EmitMov(Register::rdx, 0, code);
								Gen::EmitMovStack(Register::rax, -4, code);
								Gen::EmitMovStack(Register::rbx, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								
								Gen::EmitIdiv(Register::rbx, code);
								Gen::EmitMovStack(-4, Register::rdx, code);
								
								Gen::EmitMovStack(Register::rax, -1, code);
								Gen::EmitMovStack(Register::rdx, -2, code);
								Gen::EmitMovStack(Register::rbx, -3, code);
								
								Gen::EmitMovStack(stmt.dest, -4, code);
								break;
							}
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::And:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitAnd(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitAnd(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Or:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitOr(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitOr(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					case Operation::Xor:
						switch (stmt.source->tag) {
							case OperandTag::Register: Gen::EmitXor(stmt.dest, dynamic_cast<const RegisterOperand &>(*stmt.source).reg, code);
								break;
							case OperandTag::Immediate: Gen::EmitXor(stmt.dest, dynamic_cast<const ImmediateOperand &>(*stmt.source).value, code);
								break;
							default: return Error{"Unsopported source argument type.", statement.pos};
						}
						break;
					default: return Error{"Unsupported shorthand operation type.", stmt.pos};
				}
				break;
			}
			case StatementTag::Longhand: {
				//const auto& stmt = static_cast<const LonghandStatement&>(statement);
				return Error{"Statement not implemented in the compiler.", statement.pos};
			}
			case StatementTag::Loop: {
				const auto &stmt = dynamic_cast<const Parser::LoopStatement &>(statement);
				
				for (const auto &innerStatement: stmt.statements) {
					Error _error = (CompileStatement(*innerStatement, code, callTable));
					if (_error)
						return _error;
				}
				
				const size_t jumpBackPtr = code.length();
				Gen::EmitNop(5, code);
				Gen::WriteJump(jumpBackPtr, startPtr, code);
				
				for (const auto ptr: loopBreaks) {
					Gen::WriteJump(ptr, code.length(), code);
				}
				
				for (const auto ptr: loopContinues) {
					Gen::WriteJump(ptr, startPtr, code);
				}
				
				loopBreaks.clear();
				loopContinues.clear();
				break;
			}
			case StatementTag::Branch: {
				const auto &stmt = dynamic_cast<const Parser::BranchStatement &>(statement);
				
				size_t jumpPastElsePtr = 0;
				for (const auto &innerStatement: stmt.statements) {
					Error _error = (CompileStatement(*innerStatement, code, callTable));
					if (_error)return _error;
					
					if (!stmt.elseBlock.empty()) {
						jumpPastElsePtr = code.length();
						Gen::EmitNop(5, code);
					}
				}
				const size_t ifBlockEndPtr = code.length();
				if (!stmt.elseBlock.empty()) {
					for (const auto &innerStatement: stmt.elseBlock) {
						Error _error = (CompileStatement(*innerStatement, code, callTable));
						if (_error)return _error;
						
					}
					const size_t elseBlockEndPtr = code.length();
					
					Gen::WriteJump(jumpPastElsePtr, elseBlockEndPtr, code);
				}
				
				Gen::WriteJump(conditionalJumpPtr, ifBlockEndPtr, statement.condition->comp, conditionalJumpInverted, code);
				break;
			}
			case StatementTag::Break: {
				loopBreaks.emplace(code.length());
				Gen::EmitNop(5, code);
				break;
			}
			case StatementTag::Continue: {
				loopContinues.emplace(code.length());
				Gen::EmitNop(5, code);
				break;
			}
			case StatementTag::Return: {
				Gen::EmitReturn(code);
				break;
			}
			case StatementTag::Call: {
				const auto &stmt = dynamic_cast<const Parser::CallStatement &>(statement);
				
				callTable[code.length()] = stmt.name;
				Gen::EmitNop(5, code);
				break;
			}
			case StatementTag::Stdout: {
				const auto &stmt = dynamic_cast<const Parser::StdoutStatement &>(statement);
				
				void (*fn)(int64_t) = &Print;
				int64_t addr;
				memcpy(&addr, &fn, 8);
				
				Gen::EmitPushAllRegs(code);
				
				switch (stmt.source->tag) {
					case OperandTag::Register: {
						const auto &source = dynamic_cast<const RegisterOperand &>(*stmt.source);
						const Register callReg = source.reg != Register::rax ? Register::rax : Register::rbx;
						Gen::EmitMov(callReg, addr, code);
						if (source.reg != Register::rdi) Gen::EmitMov(Register::rdi, source.reg, code);
						Gen::EmitCall(callReg, code);
						break;
					}
					case OperandTag::Immediate: {
						const int64_t value = dynamic_cast<const ImmediateOperand &>(*stmt.source).value;
						Gen::EmitMov(Register::rax, addr, code);
						Gen::EmitMov(Register::rdi, value, code);
						Gen::EmitCall(Register::rax, code);
						break;
					}
					default: return Error{"Unsopported source argument type.", statement.pos};
				}
				
				Gen::EmitPopAllRegs(code);
				
				break;
			}
			case StatementTag::StdoutText: {
				const auto &stmt = dynamic_cast<const Parser::StdoutTextStatement &>(statement);
				
				void (*fn)(const char *, size_t) = &Print;
				int64_t addr;
				memcpy(&addr, &fn, 8);
				
				Gen::EmitNop(5, code);
				const size_t textPtr = code.length();
				code.append(reinterpret_cast<const unsigned char *>(stmt.text.data()), stmt.text.length());
				Gen::WriteJump(startPtr, code.length(), code);
				
				Gen::EmitPushAllRegs(code);
				
				Gen::EmitMov(Register::rax, addr, code);
				Gen::EmitLea(Register::rdi, textPtr, code);
				Gen::EmitMov(Register::rsi, (int64_t) stmt.text.length(), code);
				Gen::EmitCall(Register::rax, code);
				
				Gen::EmitPopAllRegs(code);
				break;
			}
			case StatementTag::Push: Gen::EmitPush(dynamic_cast<const Parser::RegisterStatement &>(statement).reg, code);
				break;
			case StatementTag::Pop: Gen::EmitPop(dynamic_cast<const Parser::RegisterStatement &>(statement).reg, code);
				break;
			default: return Error{"Statement not implemented in the compiler.", statement.pos};
		}
		
		if (statement.condition.has_value() && statement.tag != StatementTag::Branch) {
			Gen::WriteJump(conditionalJumpPtr, code.length(), statement.condition->comp, conditionalJumpInverted, code);
		}
		return Error::None;
	}
	
	[[nodiscard]]  Error CompileCondition(const Condition &condition, MachineCode &code, bool &inverted) {
		const OperandTag atag = condition.a->tag;
		const OperandTag btag = condition.b->tag;
		
		if (atag == OperandTag::Register && btag == OperandTag::Register) {
			Gen::EmitCmp(
					dynamic_cast<const RegisterOperand &>(*condition.a).reg,
					dynamic_cast<const RegisterOperand &>(*condition.b).reg,
					code, inverted
			);
		}
		else if (atag == OperandTag::Register && btag == OperandTag::Immediate) {
			Gen::EmitCmp(
					dynamic_cast<const RegisterOperand &>(*condition.a).reg,
					dynamic_cast<const ImmediateOperand &>(*condition.b).value,
					code, inverted
			);
		}
		else if (atag == OperandTag::Immediate && btag == OperandTag::Register) {
			Gen::EmitCmp(
					dynamic_cast<const ImmediateOperand &>(*condition.a).value,
					dynamic_cast<const RegisterOperand &>(*condition.b).reg,
					code, inverted
			);
		}
		else {
			return Error{"Unsupported comparison operand type combination", condition.pos};
		}
		
		return Error::None;
	}
	
	void CompileStartProcedure(MachineCode &code, std::unordered_map<size_t, std::string> &callTable) {
		Gen::EmitPushAllRegs(code);
		
		const size_t ptr = code.length();
		Gen::EmitNop(5, code);
		callTable[ptr] = "main";
		
		Gen::EmitPopAllRegs(code);
		
		Gen::EmitReturn(code);
	}
}