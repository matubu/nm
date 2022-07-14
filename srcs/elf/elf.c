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

u64	get_field(const elf_t *elf, u64 off, elf_field_t field)
{
	const int		elf_class = elf->class - 1;

	off += field.off[elf_class];
	if (off >= elf->f->len)
	{
		// TODO better error handling
		err("%s (out of bound)", elf->f->path);
		return (0);
	}

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
		// TODO check every offset, non-null string
		masked = swap_endianess(masked);
	}

	return (masked);
}

byte	*get_section_name(elf_t *elf, u64 sec_off)
{
	if (sec_off == 0)
		return (NULL);
	return (
		elf->f->ptr
		+ elf->shrtrtab
		+ get_field(elf, sec_off, SEC_NAME)
	);
}

u64		get_section_header_from_idx(elf_t *elf, u64 idx)
{
	u64	sec_off = get_elf_field(elf, ELF_SHOFF);

	return (sec_off + SEC_HEADER.size[elf->class - 1] * idx);
}

u64		get_section_header(elf_t *elf, const byte *name)
{
	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);
	
	for (u64 i = 0; i < sec_num; ++i)
	{
		if (strsame(get_section_name(elf, sec_off), name))
			return (sec_off);
		sec_off += SEC_HEADER.size[elf->class - 1];
	}
	return (0);
}

u64		get_section(elf_t *elf, const byte *name)
{
	u64	sec_header_off = get_section_header(elf, name);
	
	if (!sec_header_off)
		return (0);
	return (get_field(elf, sec_header_off, SEC_OFFSET));
}

static inline u64	get_shrtrtab(elf_t *elf)
{
	u64	names_sec_idx = get_elf_field(elf, ELF_SHSTRNDX);
	u64	sections_table_off = get_elf_field(elf, ELF_SHOFF);

	u64	names_sec_header_off = sections_table_off
		+ names_sec_idx * SEC_HEADER.size[elf->class - 1];

	return (get_field(elf, names_sec_header_off, SEC_OFFSET));
}

elf_t	*elf_from_string(const file_t *f)
{
	#define invalid_elf(elf, s) { \
		err("%s: Invalid ELF file: " s, elf->f->path); \
		free(elf); \
		return (NULL); \
	}

	elf_t	*elf = malloc(sizeof(elf_t));

	elf->f = f;
	elf->class = 1;

	// Check magic's bytes
	if (get_elf_field(elf, ELF_MAGIC) != elf_magic)
		invalid_elf(elf, "Invalid magic bytes");

	// Check the class (64 or 32 bit)
	elf->class = get_elf_field(elf, ELF_CLASS);
	if (elf->class != 1 && elf->class != 2)
		invalid_elf(elf, "Invalid class");

	// Check is big enough to contain the entire header depending on the class
	if (f->len < ELF_HEADER.size[elf->class - 1])
		invalid_elf(elf, "Header incomplete");

	// Check the endianess (Little-endian or Big-endian)
	elf->endian = get_elf_field(elf, ELF_DATA);
	if (elf->endian != 1 && elf->endian != 2)
		invalid_elf(elf, "Invalid endianness");

	// Check if the elf version is supported
	if (get_elf_field(elf, ELF_EI_VERSION) != 1 || get_elf_field(elf, ELF_VERSION) != 1)
		invalid_elf(elf, "version not supported");

	elf->shrtrtab = get_shrtrtab(elf);

	return (elf);
}