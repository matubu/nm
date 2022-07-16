#pragma once

#include "res.h"

typedef struct {
	u64		value;
	char	type;
	byte	*name;
}	symbol_t;

typedef struct {
	symbol_t	*ptr;
	u64			cnt;
}	symbols_t;

Res(symbols_t)	get_symbols(elf_t *elf);
void			sort_symbols(symbols_t *sym);
void			print_symbol(symbol_t *sym);
void			free_symbols(symbols_t *sym);