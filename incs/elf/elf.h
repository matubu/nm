#pragma once

#include "file.h"

typedef struct {
	const file_t	*f;
	int				class; // 1 == 32bit,  2 == 64bit
	int				endian; // 1 == Little-endian,  2 == Big-endian
	u64				shrtrtab;
}	elf_t;


#include "elf/fields.h"
#include "elf/utils.h"