#pragma once

#include "Types.h"

#include <string>

using namespace Compiler;

namespace Runtime {


	struct Error {
		bool error;
		std::string message;
		CodePos pos;

		operator bool() const { return error; }

	private:
		Error(const bool error, std::string message, const CodePos pos) : error{error}, message{std::move(message)}, pos{pos} {}
	public:
		Error(std::string message, const CodePos pos) : error{true}, message{std::move(message)}, pos{pos} {}
		static const Error None;
	};

	inline const Error Error::None = Error{false, std::string{}, CodePos{}};
}