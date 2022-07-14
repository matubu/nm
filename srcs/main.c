#include <unistd.h>
#include <sys/mman.h>

#include "malloc.h"
#include "io.h"
#include "elf/elf.h"
#include "file.h"
#include "flags.h"

void	nm(args_t *args, char *path)
{
	file_t	*f = read_file(path);
	if (f == NULL)
		return (sys_err("%s", path));

	elf_t	*elf = elf_from_string(f);
	if (elf == NULL)
		goto free;

	symbols_t	symbols = get_symbols(elf);
	if (!(args->flags & no_sort))
		sort_symbols(&symbols);

	if (args->file_count >= 2)
		fmt("\n%s:\n", f->path);

	if (args->flags & reverse_sort)
		for (u64 i = symbols.cnt; i--;)
			print_symbol(symbols.ptr + i);
	else
		for (u64 i = 0; i < symbols.cnt; ++i)
			print_symbol(symbols.ptr + i);

	free_symbols(&symbols);

free:
	free(elf);
	free_file(f);
}

int	main(int argc, char **argv)
{
	args_t	*args = parse_arguments(argc, argv);

	for (int i = 0; i < args->file_count; ++i)
		nm(args, args->files[i]);
}