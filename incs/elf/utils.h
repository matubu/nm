#pragma once

#include "elf/elf.h"

u64	get_field(const elf_t *elf, u64 off, elf_field_t field);
#define get_elf_field(elf, field) get_field(elf, 0, field)

byte	*get_section_name(elf_t *elf, u64 sec_off);

u64		get_section_header_from_idx(elf_t *elf, u64 idx);
u64		get_section_header(elf_t *elf, const byte *name);
u64		get_section(elf_t *elf, const byte *name);

elf_t	*elf_from_string(const file_t *f);