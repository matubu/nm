#include "elf/elf.h"
#include "int.h"
#include "io.h"

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

// TODO filter
i32	sym_filter(elf_t *elf, u64 sym_off)
{
	if (get_field(elf, sym_off, SYM_SHNDX) == SHN_ABS)
		return (0);
	
	return (1); // Seleted
}

// readelf -s elf
void	parse_elf_symbols(elf_t *elf, u64 sec_off, const char *str_table_name)
{
	u64			sym_sec_entsize = get_field(elf, sec_off, SEC_ENTSIZE);
	u64			sym_sec_off = get_field(elf, sec_off, SEC_OFFSET) + sym_sec_entsize;

	u64			sym_names_sec = get_section(elf, (byte *)str_table_name);

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

		if (sym_filter(elf, sym_sec_off))
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
	// puts("");
}

void	parse_elf_sections(elf_t *elf)
{

	u64	sec_off = get_elf_field(elf, ELF_SHOFF);
	u64	sec_num = get_elf_field(elf, ELF_SHNUM);

	for (u64 i = 0; i < sec_num; ++i)
	{
		if (get_field(elf, sec_off, SEC_TYPE) == SymbolTable)
			parse_elf_symbols(elf, sec_off, ".strtab");
		sec_off += SEC_HEADER.size[elf->class - 1];
	}
}