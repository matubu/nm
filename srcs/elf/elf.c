#include "elf/elf.h"
#include "int.h"
#include "io.h"

static inline u64	swap_endianess(u64 a)
{
	return (0
		| (a            & (u64)0XFF << (7 * 8))
		| (a >> (1 * 8) & (u64)0XFF << (6 * 8))
		| (a >> (2 * 8) & (u64)0XFF << (5 * 8))
		| (a >> (3 * 8) & (u64)0XFF << (4 * 8))
		| (a >> (4 * 8) & (u64)0XFF << (3 * 8))
		| (a >> (5 * 8) & (u64)0XFF << (2 * 8))
		| (a >> (6 * 8) & (u64)0XFF << (1 * 8))
		| (a >> (7 * 8)             << (0 * 8))
	);
}

Res(u64)	_get_field(const elf_t *elf, u64 off, elf_field_t field)
{
	Returns(u64);

	const int		elf_class = elf->class - 1;

	off += field.off[elf_class];
	if (off >= elf->f->len)
		throw("missing data");

	const u64	data = *(u64 *)(elf->f->ptr + off);
	const u64	mask_map[] = { 
		0,               (1ULL<<(1*8))-1, (1ULL<<(2*8))-1, (1ULL<<(3*8))-1,
		(1ULL<<(4*8))-1, (1ULL<<(5*8))-1, (1ULL<<(6*8))-1, (1ULL<<(7*8))-1,
		(u64)-1
	};
	const u64	mask = mask_map[field.size[elf_class]];

	u64 masked = data & mask;

	if (off >= 0x10 && CurrentEndian != elf->endian)
	{
		// TODO test on different endianess
		masked = swap_endianess(masked);
	}

	return Ok(masked);
}

Res(byte_ptr) get_section_name(elf_t *elf, u64 sec_off)
{
	Returns(byte_ptr);

	if (sec_off == 0)
		return Ok(NULL);
	return Ok(
		elf->f->ptr
		+ elf->shrtrtab
		+ get_field(elf, sec_off, SEC_NAME)
	);
}

Res(u64)		get_section_header_from_idx(elf_t *elf, u64 idx)
{
	Returns(u64);

	u64	sec_off = get_elf_field(elf, ELF_SHOFF);

	return Ok(sec_off + SEC_HEADER.size[elf->class - 1] * idx);
}

Res(u64)	get_section_header(elf_t *elf, const byte *name)
{
	Returns(u64);

	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);
	
	for (u64 i = 0; i < sec_num; ++i)
	{
		if (!cmp_bytes(unwrap(get_section_name(elf, sec_off)), name))
			return Ok(sec_off);
		sec_off += SEC_HEADER.size[elf->class - 1];
	}
	return Ok(0);
}

Res(u64)	get_section(elf_t *elf, const byte *name)
{
	Returns(u64);

	u64	sec_header_off = unwrap(get_section_header(elf, name));
	
	if (!sec_header_off)
		return Ok(0);
	return (_get_field(elf, sec_header_off, SEC_OFFSET));
}

static inline Res(u64)	get_shrtrtab(elf_t *elf)
{
	Returns(u64);

	u64	names_sec_idx = get_elf_field(elf, ELF_SHSTRNDX);
	u64	sections_table_off = get_elf_field(elf, ELF_SHOFF);

	u64	names_sec_header_off = sections_table_off
		+ names_sec_idx * SEC_HEADER.size[elf->class - 1];

	return (_get_field(elf, names_sec_header_off, SEC_OFFSET));
}

Res(elf_ptr) elf_from_string(const file_t *f)
{
	Returns(elf_ptr);

	elf_t	*elf = malloc(sizeof(elf_t));

	#define invalid_elf(elf, s) { \
		free(elf); \
		throw("invalid ELF file: " s); \
	}

	elf->f = f;
	elf->class = 1;

	// Check magic's bytes
	if (get_elf_field(elf, ELF_MAGIC) != elf_magic)
		invalid_elf(elf, "invalid magic bytes");

	// Check the class (64 or 32 bit)
	elf->class = get_elf_field(elf, ELF_CLASS);
	if (elf->class != 1 && elf->class != 2)
		invalid_elf(elf, "invalid class");

	// Check the endianess (Little-endian or Big-endian)
	elf->endian = get_elf_field(elf, ELF_DATA);
	if (elf->endian != 1 && elf->endian != 2)
		invalid_elf(elf, "invalid endianness");

	// Check if the elf version is supported
	if (get_elf_field(elf, ELF_EI_VERSION) != 1 || get_elf_field(elf, ELF_VERSION) != 1)
		invalid_elf(elf, "version not supported");

	elf->shrtrtab = unwrap(get_shrtrtab(elf));

	return Ok(elf);
}