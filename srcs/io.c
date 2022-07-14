#include "io.h"

void help()
{
	PUTS(2, "Usage: ./ft_nm [option(s)] [file(s)]");
	PUTS(2, " List symbols in [file(s)] (a.out by default).");
	PUTS(2, " The options are:");
	PUTS(2, "  -a     Display debugger-only symbols");
	PUTS(2, "  -g     Display only external symbols");
	PUTS(2, "  -p     Do not sort the symbols");
	PUTS(2, "  -r     Reverse the sense of the sort");
	PUTS(2, "  -u     Display only undefined symbols");
	PUTS(2, "nm: supported targets: x86_32, x64, object files, .so");
}