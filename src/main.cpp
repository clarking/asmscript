
#include <cstdio>
#include <cstring>
#include "runtime.h"

using namespace Runtime;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,
				"Usage: %s [FLAGS] FILE\n"
				"    --dump-tokens    Dump lexer results\n"
				"    --dump-ast       Dump parser results\n"
				"    --dump-code      Dump machine code\n"
				"    --no-exec        Do not execute compiled code\n",
				argv[0]
		);
		return 1;
	}
	
	for (int argnum = 1; argnum < argc - 1; ++argnum) {
		const char *arg = argv[argnum];
		if (strcmp(arg, "--dump-tokens") == 0) Options::flag_dumpTokens = true;
		else if (strcmp(arg, "--dump-ast") == 0) Options::flag_dumpAst = true;
		else if (strcmp(arg, "--dump-code") == 0) Options::flag_dumpCode = true;
		else if (strcmp(arg, "--no-exec") == 0) Options::flag_noExec = true;
	}
	
	auto res = RunFile(argv[argc - 1]);
	return res;
}