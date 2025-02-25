#pragma once

#include <cstdarg>
#include <string>

namespace Runtime {
	bool ReadFile(const char *filepath, std::string &out);
	
	__attribute__ ((format (printf, 1, 2)))
	std::string Format(const char *fmt, ...);
	
	__attribute__ ((format (printf, 1, 0)))
	std::string FormatV(const char *fmt, va_list args);
}