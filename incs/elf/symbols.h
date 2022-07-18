#pragma once

#include "res.h"
#include "int.h"
#include "flags.h"

typedef struct {
	u64		value;
	bool	undefined;
	char	type;
	byte	*name;
}	symbol_t;

typedef struct {
	symbol_t	*ptr;
	u64			cnt;
}	symbols_t;

Res(symbols_t)	get_symbols(args_t *args, elf_t *elf);
void			sort_symbols(symbols_t *sym);
void			print_symbol(elf_t *elf, symbol_t *sym);
void			free_symbols(symbols_t *sym);