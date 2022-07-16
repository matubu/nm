#pragma once

#include "elf/elf.h"
#include "res.h"

Res(u64)	_get_field(const elf_t *elf, u64 off, elf_field_t field);
#define		get_field(...) unwrap(_get_field(__VA_ARGS__))
#define		get_elf_field(elf, field) get_field(elf, 0, field)

Res(byte_ptr)	get_section_name(elf_t *elf, u64 sec_off);

Res(u64)		get_section_header_from_idx(elf_t *elf, u64 idx);
Res(u64)		get_section_header(elf_t *elf, const byte *name);
Res(u64)		get_section(elf_t *elf, const byte *name);

Res(elf_ptr)	elf_from_string(const file_t *f);