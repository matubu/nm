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
	sort_symbols(&symbols);

	if (args->file_count >= 2)
		printf("\n%s:\n", f->path);

	print_symbols(&symbols);
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