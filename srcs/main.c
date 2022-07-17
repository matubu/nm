#include <unistd.h>
#include <sys/mman.h>

#include "malloc.h"
#include "io.h"
#include "elf/elf.h"
#include "file.h"
#include "flags.h"

void	nm(args_t *args, char *path)
{
	Res(file_ptr)	f = InitRes(file_ptr, NULL);
	Res(elf_ptr)	elf = InitRes(elf_ptr, NULL);
	Res(symbols_t)	symbols = InitRes(symbols_t, ((symbols_t){ .ptr = NULL, .cnt = 0 }));

	f = read_file(path);
	catch(f, {
		err("%s: %s", path, f.err);
		goto free;
	});

	elf = elf_from_string(f.data);
	catch(elf, {
		err("%s: %s", path, elf.err);
		goto free;
	});

	symbols = get_symbols(elf.data);
	catch(symbols, {
		err("%s: %s", path, elf.err);
		goto free;
	});

	if (!(args->flags & no_sort))
		sort_symbols(&symbols.data);

	if (args->file_count >= 2)
		fmt("\n%s:\n", path);

	if (args->flags & reverse_sort)
		for (u64 i = symbols.data.cnt; i--;)
			print_symbol(symbols.data.ptr + i);
	else
		for (u64 i = 0; i < symbols.data.cnt; ++i)
			print_symbol(symbols.data.ptr + i);


free:
	free(elf.data);
	free_symbols(&symbols.data);
	free_file(f.data);
}

int	main(int argc, char **argv)
{
	Res(args_t)	args = parse_arguments(argc, argv);

	catch(args, {
		err("%s", args.err);
		return (1);
	});

	for (int i = 0; i < args.data.file_count; ++i)
		nm(&args.data, args.data.files[i]);
	
	free_arguments(&args.data);
}