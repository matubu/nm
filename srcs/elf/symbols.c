#include "elf/elf.h"
#include "flags.h"
#include "int.h"
#include "io.h"
#include "str.h"

static inline char	upcase(char c, int upcase)
{
	return (c + (upcase ? ('A' - 'a') : 0));
}

// https://linux.die.net/man/1/nm
// https://www.man7.org/linux/man-pages/man1/nm.1.html
// http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf
// https://stackoverflow.com/questions/15225346/how-to-display-the-symbols-type-like-the-nm-command
// https://github.com/bhm-heddy/42Project_nm_elf/blob/main/srcs/flags.c
// https://medium.com/a-42-journey/nm-otool-everything-you-need-to-know-to-build-your-own-7d4fef3d7507
// TODO finish get_symbol_type
static inline Res(char)	get_symbol_type(elf_t *elf, u64 sym_off)
{
	Returns(char);

	bool	global = (SYM_BIND(get_field(elf, sym_off, SYM_INFO)) & SYM_BIND_GLOBAL) ? true : false;
	u64		sym_type = SYM_TYPE(get_field(elf, sym_off, SYM_INFO));
	u64		rel = get_field(elf, sym_off, SYM_REL);


	if (SYM_BIND(get_field(elf, sym_off, SYM_INFO)) & SYM_BIND_WEAK)
	{
		if (sym_type & SYM_TYPE_OBJECT)
			return Ok(upcase('v', global)); // is a weak object
		else
			return Ok(upcase('w', global)); // is a weak symbol
	}

	if (rel == SYM_REL_ABS)
		return Ok('A'); // absolute
	if (rel == SYM_REL_COMMON)
		return Ok(upcase('c', global)); // common
	if (rel == SYM_REL_UNDEF)
		return Ok('U'); // is undefined

	u64		rel_off = unwrap(get_section_header_from_idx(elf, rel));
	u64		rel_type = get_field(elf, rel_off, SEC_TYPE);
	u64		rel_flags = get_field(elf, rel_off, SEC_FLAGS);

	if (rel_type == SEC_TYPE_NOBITS && rel_flags == (SEC_FLAG_ALLOC | SEC_FLAG_WRITE))
		return Ok(upcase('b', global)); // in uninitialized data section (bss)
	// return (upcase('g', global)); // in a data section for small objects
	// return ('I'); // an indirect reference to another symbol
	// return ('N'); // a debugging symbol
	if (rel_type == SEC_TYPE_PROGBITS && rel_flags == 0)
		return Ok('n'); // in the read-only data section
	// https://www.zyma.me/post/stack-unwind-intro/
	// return ('p'); // in a stack unwind section (used to handle try catch block)
	// return (upcase('s', global)); // in a data section for small objects (same as g ?)
	if (rel_type == SEC_TYPE_PROGBITS && rel_flags == (SEC_FLAG_ALLOC | SEC_FLAG_EXECINSTR))
		return Ok(upcase('t', global)); // in text (code) section
	if (rel_flags == SYM_BIND_GNU_UNIQUE)
		return Ok('u'); // a unique global symbol
	if (rel_flags == (SEC_FLAG_ALLOC | SEC_FLAG_WRITE))
		return Ok(upcase('d', global)); // in the data section
	if (rel_type == SEC_TYPE_PROGBITS && rel_flags == (SEC_FLAG_ALLOC | SEC_FLAG_EXECINSTR))
		return Ok('i'); // in a section specific to the implementation of DLLs
	if (!(rel_flags & SEC_FLAG_WRITE))
		return Ok(upcase('r', global)); // in a read only data section
	// return ('-'); // is a stabs symbol in an a.out object file
	return Ok('?'); // unknown
}

static inline Res(i32)	sym_filter(args_t *args, elf_t *elf, u64 sym_off)
{
	Returns(i32);

	if (!get_field(elf, sym_off, SYM_NAME))
		return Ok(0);
	
	if (!(args->flags & debugger_symbols)
		&& (SYM_TYPE(get_field(elf, sym_off, SYM_INFO)) == SYM_TYPE_FILE))
		return Ok(0);
	
	if ((args->flags & only_global_symbols)
		&& !(SYM_BIND(get_field(elf, sym_off, SYM_INFO)) & (SYM_BIND_GLOBAL | SYM_BIND_WEAK)))
		return Ok(0);
	
	if ((args->flags & only_undefined_symbols)
		&& (get_field(elf, sym_off, SYM_REL) != SYM_REL_UNDEF))
		return Ok(0);

	return Ok(1);
}

// readelf -s elf
static inline Res(symbols_t)	parse_elf_symbols(args_t *args, elf_t *elf, u64 sec_off, u64 sym_names_sec)
{
	Returns(symbols_t);

	u64			sym_sec_entsize = get_field(elf, sec_off, SEC_ENTSIZE);
	u64			sym_sec_off = get_field(elf, sec_off, SEC_OFFSET) + sym_sec_entsize;

	u64			cnt = (get_field(elf, sec_off, SEC_SIZE) / sym_sec_entsize) - 1;

	symbols_t	sym;
	sym.cnt = 0;
	sym.ptr = malloc(sizeof(symbol_t) * cnt);

	while (cnt--)
	{
		if (unwrap(sym_filter(args, elf, sym_sec_off)))
		{
			sym.ptr[sym.cnt].value = get_field(elf, sym_sec_off, SYM_VALUE);
			sym.ptr[sym.cnt].type = unwrap(get_symbol_type(elf, sym_sec_off));
			sym.ptr[sym.cnt].name = elf->f->ptr + sym_names_sec + get_field(elf, sym_sec_off, SYM_NAME);
			++sym.cnt;
		}
		sym_sec_off += sym_sec_entsize;
	}

	return Ok(sym);
}

Res(symbols_t)	get_symbols(args_t *args, elf_t *elf)
{
	Returns(symbols_t);

	u64			sym_sec = unwrap(get_section_header(elf, (byte *)".symtab"));
	u64			sym_names_sec = unwrap(get_section(elf, (byte *)".strtab"));

	if (sym_sec == 0 || sym_names_sec == 0)
	{
		return Ok(((symbols_t){ .cnt = 0, .ptr = NULL }));
	};
	return Ok(unwrap(parse_elf_symbols(args, elf, sym_sec, sym_names_sec)));
}

void	sort_symbols(symbols_t *sym)
{
	bool	sorted = false;

	while (!sorted)
	{
		sorted = true;
		for (u64 i = 1; i < sym->cnt; ++i)
		{
			if (cmp_bytes(sym->ptr[i-1].name, sym->ptr[i].name) > 0)
			{
				symbol_t	tmp = sym->ptr[i];
				sym->ptr[i] = sym->ptr[i-1];
				sym->ptr[i-1] = tmp;
				sorted = false;
			}
		}
	}
}

void	print_symbol(symbol_t *sym)
{
	fmt("%x %c %s\n", sym->value, sym->type, sym->name);
}


void	free_symbols(symbols_t *sym)
{
	free(sym->ptr);
}