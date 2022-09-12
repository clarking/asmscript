#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cstddef>

namespace Compiler {

	enum class Register : uint8_t {
		rax = 0x00,
		rbx = 0x03,
		rcx = 0x01,
		rdx = 0x02,
		rsi = 0x06,
		rdi = 0x07,
		rbp = 0x05,
		r8 = 0x08,
		r9 = 0x09,
		r10 = 0x0A,
		r11 = 0x0B,
		r12 = 0x0C,
		r13 = 0x0D,
		r14 = 0x0E,
		r15 = 0x0F,
	};

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
		StdoutText, // StdoutTextStatement
		Push,       // RegisterStatement
		Pop,        // RegisterStatement
	};

	enum class OperandTag {
		Register,
		Immediate,
	};

	enum class Operation {
		Add,
		Sub,
		Mul,
		Div,
		Mod,
		And,
		Or,
		Xor,
	};

	enum class Comparison : uint8_t {
		LessThan = 0x7C,
		LessEquals = 0x7E,
		GreaterThan = 0x7F,
		GreaterEquals = 0x7D,
		Equals = 0x74,
		NotEquals = 0x75,
	};

	struct CodePos {

		size_t line;
		size_t col;

		CodePos() = default;

		CodePos(const size_t line, const size_t col) : line{line}, col{col} {}
	};

	struct CodePtr {
		const char *ptr;
		CodePos pos;

		explicit CodePtr(const char *const ptr) : ptr{ptr}, pos{1, 1} {}

		const char &operator[](const size_t index) const { return ptr[index]; }

		CodePtr &operator+=(const size_t count) {
			for (size_t i = 0; i < count; ++i, ++ptr) {
				const char c = *ptr;
				if (c == '\n') {
					pos.line += 1;
					pos.col = 1;
				}
				else {
					pos.col += 1;
				}
			}
			return *this;
		}
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

}