#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "elf.h"
#include "file.h"


#include <stdio.h>

// typedef enum {
// 	debugger_symbols      =0b00001, // -a
// 	only_global_symbols   =0b00010, // -g
// 	only_undefined_symbols=0b00100, // -u
// 	reverse_sort          =0b01000, // -r
// 	no_sort               =0b10000, // -p
// }	flags_e;

void	err(char *file, char *err)
{
	printf("nm: \033[91merror:\033[0m %s: %s\n", file, err);
}

void	sys_err(char *file)
{
	printf("nm: \033[91merror:\033[0m %s: %s\n", file, strerror(errno));
}

u64	swap_endianess(u64 a)
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

// addr
u64	get_field(const elf_t *elf, u64 off, elf_field_t field)
{
	const int		elf_class = elf->class - 1;

	off += field.off[elf_class];
	if (off > elf->f->len)
	{
		err(elf->f->path, "(out of bound)");
		printf("out\n");
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
		// TODO test
		printf("Swap endian\n");
		masked = swap_endianess(masked);
	}

	return (masked);
}
#define get_elf_field(elf, field) get_field(elf, 0, field)

#define invalid_elf(elf, s) { \
	err(elf->f->path, "Invalid ELF file: " s); \
	free(elf); \
	return (NULL); \
}

void	print_flags(u64 flags, const mask_mapping_t *mapping)
{
	int	printed = 0;

	for (int i = 0; mapping[i].mask; ++i)
	{
		if (flags & mapping[i].mask)
		{
			if (printed++)
				printf(", ");
			else
				printf("[ ");
			printf("\033[32m%s\033[0m", mapping[i].mapped);
		}
	}
	if (printed == 0)
		puts("\033[30mNo flags\033[0m");
	else
		puts(" ]");
}

void	parse_elf_symbols(elf_t *elf, u64 sec_off)
{
	u64	sym_sec_off = get_field(elf, sec_off, SEC_OFFSET);
	u64	sym_sec_end = sym_sec_off + get_field(elf, sec_off, SEC_SIZE);
	u64	sym_sec_entsize = get_field(elf, sec_off, SEC_ENTSIZE);

	printf("addr=%lu\n", get_field(elf, sec_off, SEC_ADDR));
	printf("off=%lu  0x%lx 0x%zx\n", sym_sec_off, sym_sec_entsize, SYM_HEADER.size[elf->class - 1]);
	while (sym_sec_off < sym_sec_end)
	{
		printf("nameoff=%lu\n", get_field(elf, sym_sec_off, SYM_NAME));
		if (get_field(elf, sym_sec_off, SYM_NAME))
			printf("%s\n", elf->f->ptr + get_field(elf, sym_sec_off, SYM_NAME));
		//sym_sec_off += sym_sec_entsize;
		sym_sec_off += /*sym_sec_entsize;*/SYM_HEADER.size[elf->class - 1];
	}
	// for (int i = 0; i < ; ++i)
	// {
	// 	sym_sec_off += sym_sec_entsize;
	// }
}

void	parse_elf_sections(elf_t *elf)
{
	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);

	u64	names_sec_idx = get_elf_field(elf, ELF_SHSTRNDX);
	u64	names_sec_header_off = sec_off
		+ names_sec_idx * SEC_HEADER.size[elf->class - 1];
	u64	names_sec_off
		= get_field(elf, names_sec_header_off, SEC_OFFSET);

	for (u64 i = 0; i < sec_num; ++i)
	{
		u64	name_off = get_field(elf, sec_off, SEC_NAME);
		printf("\033[94m%s\033[0m (%s)\n",
			elf->f->ptr + names_sec_off + name_off,
			GET_STRING_MAPPING(sec_type, get_field(elf, sec_off, SEC_TYPE))
		);
		print_flags(get_field(elf, sec_off, SEC_FLAGS), sec_flags);
		puts("");

		if (get_field(elf, sec_off, SEC_TYPE) == SymbolTable)
			parse_elf_symbols(elf, sec_off);

		sec_off += SEC_HEADER.size[elf->class - 1];
	}
}

elf_t	*elf_from_string(const file_t *f)
{
	elf_t	*elf;

	elf = malloc(sizeof(elf_t));
	if (elf == NULL)
		return (NULL);

	elf->f = f;
	elf->class = 1;

	if (get_elf_field(elf, ELF_MAGIC) != elf_magic) // Check magic
		invalid_elf(elf, "invalid magic bytes");

	elf->class = get_elf_field(elf, ELF_CLASS); // Check valid class
	if (elf->class != 1 && elf->class != 2)
		invalid_elf(elf, "invalid class");

	if (f->len < ELF_HEADER.size[elf->class - 1]) // Header too small
		invalid_elf(elf, "header incomplete");

	elf->endian = get_elf_field(elf, ELF_DATA); // Check valid endian
	if (elf->endian != 1 && elf->endian != 2)
		invalid_elf(elf, "invalid endianness");
	
	if (get_elf_field(elf, ELF_EI_VERSION) != 1 || get_elf_field(elf, ELF_VERSION) != 1)
		invalid_elf(elf, "unsupported elf version");

	printf("currentEndian %s\n", elf_data[CurrentEndian]);

	printf("%s (%s) %s\n",
		GET_STRING_MAPPING(elf_class, get_elf_field(elf, ELF_CLASS)),
		GET_STRING_MAPPING(elf_data, get_elf_field(elf, ELF_DATA)),
		GET_STRING_MAPPING(elf_osabi, get_elf_field(elf, ELF_OSABI))
	);

	printf("%s\n", GET_STRING_MAPPING(elf_type, get_elf_field(elf, ELF_TYPE)));
	printf("%s\n", GET_STRING_MAPPING(elf_machine, get_elf_field(elf, ELF_MACHINE)));

	puts("");
	parse_elf_sections(elf);

	return (elf);
}

void	nm(char *path)
{
	printf("nm(\033[94m%s\033[0m) ->\n", path);

	file_t	*f = read_file(path);
	if (f == NULL)
		return (sys_err(path));

	// puts_string(f);
	elf_from_string(f);

	free_file(f);
}

int	main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
		nm(argv[i]);
}