#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "elf.h"

#include <stdio.h>

#define LittleEndian 1
#define BigEndian 2
#define CurrentEndian (*(char *)((int []){1}) == 1 ? LittleEndian : BigEndian)

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

static inline file_t	*new_file_t(char *path, byte *ptr, size_t len)
{
	file_t	*f = malloc(sizeof(file_t));
	if (f == NULL)
		return (NULL);

	f->path = path;
	f->ptr = ptr;
	f->len = len;
	return (f);
}

void	puts_string(const file_t *s)
{
	write(1, s->ptr, s->len);
	write(1, "\n", 1);
}

void	free_string(file_t *s)
{
	munmap(s->ptr, s->len);
	free(s);
}

file_t	*read_file(char *path)
{
	int	fd = open(path, O_RDONLY);
	struct stat info;

	if (fd < 0)
		return (NULL);
	if (fstat(fd, &info) < 0)
		return (NULL);
	if (S_ISDIR(info.st_mode))
	{
		errno = EISDIR;
		return (NULL);
	}

	void	*ptr = mmap(NULL, info.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED)
		return NULL;

	return (new_file_t(path, ptr, info.st_size));
}

int	starts_with(const file_t *f, byte *p)
{
	printf("starts_with(%x, %x)\n", *f->ptr, *p);
	for (size_t i = 0; p[i]; ++i)
		if (i >= f->len || p[i] != f->ptr[i])
			return (0);
	return (1);
}

// addr
uint64_t	get_field(const elf_t *elf, uint64_t off, elf_field_t field)
{
	const int		elf_class = elf->class - 1;

	off += field.off[elf_class];
	if (off > elf->f->len)
	{
		err(elf->f->path, "(out of bound)");
		printf("out\n");
		return (0);
	}

	const uint64_t	data = *(uint64_t *)(elf->f->ptr + off);
	const uint64_t	mask_map[] = { 
		0,           (1ULL<<(1*8))-1, (1ULL<<(2*8))-1, (1ULL<(3*8))-1,
		(1ULL<<(4*8))-1, (1ULL<<(5*8))-1, (1ULL<<(6*8))-1, (1ULL<<(7*8))-1,
		(uint64_t)-1
	};
	const uint64_t	mask = mask_map[field.size[elf_class]];

	uint64_t masked = data & mask;

	if (off >= 0x10 && CurrentEndian != elf->endian)
	{
		// TODO test
		// Swap endianess
		printf("Swap endian\n");
		masked = 0
			| (masked >> (0 * 8) & (uint64_t)0XFF << (7 * 8))
			| (masked >> (1 * 8) & (uint64_t)0XFF << (6 * 8))
			| (masked >> (2 * 8) & (uint64_t)0XFF << (5 * 8))
			| (masked >> (3 * 8) & (uint64_t)0XFF << (4 * 8))
			| (masked >> (4 * 8) & (uint64_t)0XFF << (3 * 8))
			| (masked >> (5 * 8) & (uint64_t)0XFF << (2 * 8))
			| (masked >> (6 * 8) & (uint64_t)0XFF << (1 * 8))
			| (masked >> (7 * 8) & (uint64_t)0XFF << (0 * 8))
		;
	}

	return (masked);
}
#define get_elf_field(elf, field) get_field(elf, 0, field)

#define invalid_elf(elf, s) { \
	err(elf->f->path, "Invalid ELF file: " s); \
	free(elf); \
	return (NULL); \
}

void	print_flags(uint64_t flags, mask_mapping_t *mapping)
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

void	parse_elf_sections(elf_t *elf)
{
	uint64_t	sec_off = get_elf_field(elf, ELF_SHOFF);
	uint64_t	sec_num = get_elf_field(elf, ELF_SHNUM);

	uint64_t	names_sec_idx = get_elf_field(elf, ELF_SHSTRNDX);
	uint64_t	names_sec_header_off = sec_off
		+ names_sec_idx * SEC_HEADER.size[elf->class - 1];
	uint64_t	names_sec_off
		= get_field(elf, names_sec_header_off, SEC_OFFSET);

	for (uint64_t i = 0; i < sec_num; ++i)
	{
		uint64_t	name_off = get_field(elf, sec_off, SEC_NAME);
		printf("\033[94m%s\033[0m (%s)\n",
			elf->f->ptr + names_sec_off + name_off,
			GET_STRING_MAPPING(sec_type, get_field(elf, sec_off, SEC_TYPE))
		);
		print_flags(get_field(elf, sec_off, SEC_FLAGS), sec_flags);
		puts("");

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

	free_string(f);
}

int	main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
		nm(argv[i]);
}