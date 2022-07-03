#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "elf.h"
#include "file.h"


#include <stdio.h>

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

int strsame(const byte *a, const byte *b)
{
	while (*a && *a == *b)
	{
		++a;
		++b;
	}
	return (*a == *b);
}

byte	*get_section_name(elf_t *elf, u64 sec_off)
{
	return (
		elf->f->ptr
		+ elf->names_sec_off
		+ get_field(elf, sec_off, SEC_NAME)
	);
}

u64		find_section(elf_t *elf, const byte *name)
{
	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);
	
	for (u64 i = 0; i < sec_num; ++i)
	{
		if (strsame(get_section_name(elf, sec_off), name))
			return (get_field(elf, sec_off, SEC_OFFSET));
		sec_off += SEC_HEADER.size[elf->class - 1];
	}
	return (0);
}

u64	get_names_sec_off(elf_t *elf)
{
	u64	names_sec_idx = get_elf_field(elf, ELF_SHSTRNDX);
	u64	sections_table_off = get_elf_field(elf, ELF_SHOFF);

	u64	names_sec_header_off = sections_table_off
		+ names_sec_idx * SEC_HEADER.size[elf->class - 1];

	return (get_field(elf, names_sec_header_off, SEC_OFFSET));
}

char	upcase(char c, int upcase)
{
	return (c + (upcase ? ('A' - 'a') : 0));
}

typedef struct {
	u64		value;
	char	type;
	byte	*name;
}	symbol_t;

typedef struct {
	symbol_t	*ptr;
	u64			cnt;
}	symbols_t;

// https://linux.die.net/man/1/nm
// https://www.man7.org/linux/man-pages/man1/nm.1.html

char	get_symbol_type(elf_t *elf, u64 sym_off)
{
	int	global = ST_BIND(get_field(elf, sym_off, SYM_INFO)) & STB_GLOBAL;
	if (get_field(elf, sym_off, SYM_SHNDX) == SHN_ABS)
		return ('A'); // absolute
	// return (upcase('b', global)); // in uninitialized data section (bss)
	if (get_field(elf, sym_off, SYM_SHNDX) == SHN_COMMON)
		return (upcase('c', global)); // common
	// return (upcase('d', global)); // in the data section
	// return (upcase('g', global)); // in a data section for small objects
	// return ('i'); // in a section specific to the implementation of DLLs
	// return ('I'); // an indirect reference to another symbol
	// return ('N'); // a debugging symbol
	// return ('n'); // in the read-only data section
	// return ('p'); // in a stack unwind section
	// return (upcase('r', global)); // in a read only data section
	// return (upcase('s', global)); // in a data section for small objects (same as g ?)
	// return (upcase('t', global)); // in text (code) section
	// return ('u'); // a unique global symbol
	if (ST_BIND(get_field(elf, sym_off, SYM_INFO)) & STB_WEAK)
	{
		if (ST_TYPE(get_field(elf, sym_off, SYM_INFO)) & STT_OBJECT)
			return (upcase('v', global)); // is a weak object
		else
			return (upcase('w', global)); // is a weak symbol
	}
	if (get_field(elf, sym_off, SYM_SHNDX) == SHN_UNDEF)
		return ('U'); // is undefined
	// return ('-'); // is a stabs symbol in an a.out object file
	return ('?'); // unknown
}

i64		sym_cmp(byte *a, byte *b)
{
	if (a == NULL || b == NULL)
		return (a - b);

	while (*a != '\0' && *a == *b)
	{
		++a;
		++b;
	}
	return (*a - *b);
}

void	sym_sort(symbols_t *sym)
{
	int	unsorted = 1;

	while (unsorted)
	{
		unsorted = 0;
		for (u64 i = 1; i < sym->cnt; ++i)
		{
			if (sym_cmp(sym->ptr[i-1].name, sym->ptr[i].name) > 0)
			{
				symbol_t	tmp = sym->ptr[i];
				sym->ptr[i] = sym->ptr[i-1];
				sym->ptr[i-1] = tmp;
				unsorted = 1;
			}
		}
	}
}

// readelf -s elf
void	parse_elf_symbols(elf_t *elf, u64 sec_off, const char *str_table_name)
{
	u64			sym_sec_entsize = get_field(elf, sec_off, SEC_ENTSIZE);
	u64			sym_sec_off = get_field(elf, sec_off, SEC_OFFSET) + sym_sec_entsize;

	u64			sym_names_sec = find_section(elf, (byte *)str_table_name);

	u64			cnt = (get_field(elf, sec_off, SEC_SIZE) / sym_sec_entsize) - 1;

	symbols_t	sym;
	sym.cnt = 0;
	sym.ptr = malloc(sizeof(symbol_t) * cnt);

	while (cnt--)
	{
		sym.ptr[sym.cnt].value = get_field(elf, sym_sec_off, SYM_VALUE);
		sym.ptr[sym.cnt].type = get_symbol_type(elf, sym_sec_off);
		sym.ptr[sym.cnt].name = NULL;
		if (get_field(elf, sym_sec_off, SYM_NAME))
			sym.ptr[sym.cnt].name = elf->f->ptr + sym_names_sec + get_field(elf, sym_sec_off, SYM_NAME);

		if (get_field(elf, sym_sec_off, SYM_SHNDX) != SHN_ABS)
			++sym.cnt;
		sym_sec_off += sym_sec_entsize;
	}

	sym_sort(&sym);

	for (u64 i = 0; i < sym.cnt; ++i)
	{
		if (sym.ptr[i].value)
			printf("%016"PRIx64" ", sym.ptr[i].value);
		else
			printf("%16s ", "");

		printf("%c %s\n", sym.ptr[i].type, sym.ptr[i].name);
	}
	puts("");
}

void	parse_elf_sections(elf_t *elf)
{
	elf->names_sec_off = get_names_sec_off(elf);

	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);

	for (u64 i = 0; i < sec_num; ++i)
	{
		printf("\033[94m%s\033[0m (%s)\n",
			get_section_name(elf, sec_off),
			GET_STRING_MAPPING(sec_type, get_field(elf, sec_off, SEC_TYPE))
		);
		print_flags(get_field(elf, sec_off, SEC_FLAGS), sec_flags);
		puts("");

		if (get_field(elf, sec_off, SEC_TYPE) == SymbolTable)
			parse_elf_symbols(elf, sec_off, ".strtab");
		// if (get_field(elf, sec_off, SEC_TYPE) == DynamicLinkerSymbolTable)
		// 	parse_elf_symbols(elf, sec_off, ".dynstr");

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

	// Check magic's bytes
	if (get_elf_field(elf, ELF_MAGIC) != elf_magic)
		invalid_elf(elf, "invalid magic bytes");

	// Check the class (64 or 32 bit)
	elf->class = get_elf_field(elf, ELF_CLASS);
	if (elf->class != 1 && elf->class != 2)
		invalid_elf(elf, "invalid class");

	// Check is big enough to contain the entire header depending on the class
	if (f->len < ELF_HEADER.size[elf->class - 1])
		invalid_elf(elf, "header incomplete");

	// Check the endianess (Little-endian or Big-endian)
	elf->endian = get_elf_field(elf, ELF_DATA);
	if (elf->endian != 1 && elf->endian != 2)
		invalid_elf(elf, "invalid endianness");

	// Check if the elf version is supported
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