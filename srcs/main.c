#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "io.h"
#include "elf/elf.h"
#include "file.h"

void	nm(char *path, int show_filename)
{
	file_t	*f = read_file(path);
	if (f == NULL)
		return (sys_err(path));

	elf_t	*elf = elf_from_string(f);
	if (elf == NULL)
		goto free;

	symbols_t	symbols = get_symbols(elf);
	sort_symbols(&symbols);

	if (show_filename)
		printf("\n%s:\n", f->path);

	print_symbols(&symbols);
	free_symbols(&symbols);

free:
	free(elf);
	free_file(f);
}

int	main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
		nm(argv[i], argc > 2);
}