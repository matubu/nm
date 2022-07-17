#pragma once

#include "res.h"

typedef enum {
	debugger_symbols      = 0b00001, // -a
	only_global_symbols   = 0b00010, // -g
	only_undefined_symbols= 0b00100, // -u
	reverse_sort          = 0b01000, // -r
	no_sort               = 0b10000, // -p
}	flags_e;

typedef struct {
	flags_e	flags;
	char	**files;
	int		file_count;
}	args_t;

NewResType(args_t);
NewResType(flags_e);

Res(args_t)	parse_arguments(int argc, char **argv);
void			free_arguments(args_t *args);