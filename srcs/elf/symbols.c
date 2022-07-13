#include "elf/elf.h"
#include "int.h"
#include "io.h"

static inline char	upcase(char c, int upcase)
{
	return (c + (upcase ? ('A' - 'a') : 0));
}

static inline bool is_in_section(elf_t *elf, u64 sym_off, char *sec_name)
{
	u64 sym_addr = get_field(elf, sym_off, SYM_VALUE);

	u64 sec_header = get_section_header(elf, (byte *)sec_name);
	if (sec_header == 0)
		return (false);
	u64 sec_start = get_field(elf, sec_header, SEC_ADDR);
	u64 sec_size = get_field(elf, sec_header, SEC_SIZE);
	u64 sec_end = sec_start + sec_size;

	return (sym_addr >= sec_start && sym_addr < sec_end);
}

// https://linux.die.net/man/1/nm
// https://www.man7.org/linux/man-pages/man1/nm.1.html
// http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf
// TODO finish this
static inline char	get_symbol_type(elf_t *elf, u64 sym_off)
{
	int	global = SYM_BIND(get_field(elf, sym_off, SYM_INFO)) & SYM_BIND_GLOBAL;
	// int	rel = get_field(elf, sym_sec_off, SYM_REL);


	if (get_field(elf, sym_off, SYM_REL) == SYM_REL_ABS)
		return ('A'); // absolute
	// if (is_in_section(elf, sym_off, ".bss"))
	// 	return (upcase('b', global)); // in uninitialized data section (bss)
	if (get_field(elf, sym_off, SYM_REL) == SYM_REL_COMMON)
		return (upcase('c', global)); // common
	// if (is_in_section(elf, sym_off, ".data"))
	// 	return (upcase('d', global)); // in the data section
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
	if (SYM_BIND(get_field(elf, sym_off, SYM_INFO)) & SYM_BIND_WEAK)
	{
		if (SYM_TYPE(get_field(elf, sym_off, SYM_INFO)) & SYM_TYPE_OBJECT)
			return (upcase('v', global)); // is a weak object
		else
			return (upcase('w', global)); // is a weak symbol
	}
	if (get_field(elf, sym_off, SYM_REL) == SYM_REL_UNDEF)
		return ('U'); // is undefined
	// return ('-'); // is a stabs symbol in an a.out object file
	return ('?'); // unknown
}

static inline i64		sym_cmp(byte *a, byte *b)
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

static inline i32	sym_filter(elf_t *elf, u64 sym_off)
{
	if (!get_field(elf, sym_off, SYM_NAME))
		return (0);
	
	if (SYM_TYPE(get_field(elf, sym_off, SYM_INFO)) == SYM_TYPE_FILE)
		return (0);

	return (1);
}

// readelf -s elf
static inline symbols_t	parse_elf_symbols(elf_t *elf, u64 sec_off, u64 sym_names_sec)
{
	u64			sym_sec_entsize = get_field(elf, sec_off, SEC_ENTSIZE);
	u64			sym_sec_off = get_field(elf, sec_off, SEC_OFFSET) + sym_sec_entsize;

	u64			cnt = (get_field(elf, sec_off, SEC_SIZE) / sym_sec_entsize) - 1;

	symbols_t	sym;
	sym.cnt = 0;
	sym.ptr = malloc(sizeof(symbol_t) * cnt);

	while (cnt--)
	{
		if (sym_filter(elf, sym_sec_off))
		{
			sym.ptr[sym.cnt].value = get_field(elf, sym_sec_off, SYM_VALUE);
			sym.ptr[sym.cnt].type = get_symbol_type(elf, sym_sec_off);
			sym.ptr[sym.cnt].name = elf->f->ptr + sym_names_sec + get_field(elf, sym_sec_off, SYM_NAME);
			++sym.cnt;
		}
		sym_sec_off += sym_sec_entsize;
	}

	return (sym);
}

symbols_t	get_symbols(elf_t *elf)
{
	u64			sym_sec = get_section_header(elf, (byte *)".symtab");
	u64			sym_names_sec = get_section(elf, (byte *)".strtab");

	if (sym_sec == 0 || sym_names_sec == 0)
	{
		return ((symbols_t){
			.cnt = 0,
			.ptr = NULL
		});
	};
	return (parse_elf_symbols(elf, sym_sec, sym_names_sec));
}

void	sort_symbols(symbols_t *sym)
{
	bool	sorted = false;

	while (!sorted)
	{
		sorted = true;
		for (u64 i = 1; i < sym->cnt; ++i)
		{
			if (sym_cmp(sym->ptr[i-1].name, sym->ptr[i].name) > 0)
			{
				symbol_t	tmp = sym->ptr[i];
				sym->ptr[i] = sym->ptr[i-1];
				sym->ptr[i-1] = tmp;
				sorted = false;
			}
		}
	}
}

void	print_symbols(symbols_t *sym)
{
	for (u64 i = 0; i < sym->cnt; ++i)
	{
		if (sym->ptr[i].value)
			printf("%016"PRIx64" ", sym->ptr[i].value);
		else
			printf("%16s ", "");

		printf("%c %s\n", sym->ptr[i].type, sym->ptr[i].name);
	}
}


void	free_symbols(symbols_t *sym)
{
	free(sym->ptr);
}