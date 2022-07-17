#pragma once

#include "file.h"

typedef struct {
	const file_t	*f;
	int				class; // 1 == 32bit,  2 == 64bit
	int				endian; // 1 == Little-endian,  2 == Big-endian
	u64				shrtrtab;
}	elf_t;

typedef	elf_t	*elf_ptr;

#include "elf/fields.h"
#include "elf/utils.h"
#include "elf/symbols.h"

#include "res.h"

NewResType(char);
NewResType(i32);
NewResType(u64);
NewResType(byte_ptr);
NewResType(elf_ptr);
NewResType(symbols_t);
