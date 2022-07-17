#include "flags.h"
#include "malloc.h"

flags_e	flags_mapping[256] = {
	['a'] = debugger_symbols,
	['g'] = only_global_symbols,
	['u'] = only_undefined_symbols,
	['r'] = reverse_sort,
	['p'] = no_sort
};

Res(flags_e)	parse_flag(char *s)
{
	Returns(flags_e);

	flags_e	flag = 0;
	while (*++s)
	{
		if (flags_mapping[(byte)*s] == 0)
		{
			err("invalid option -- '%c'", *s);
			help();
			throw("exiting because of invalid arguments");
		}
		else
			flag |= flags_mapping[(byte)*s];
	}
	return Ok(flag);
}

Res(args_t)	parse_arguments(int argc, char **argv)
{
	Returns(args_t);

	args_t	args;

	args.flags = 0;
	args.files = malloc((argc < 2 ? 2 : argc) * sizeof(char *));
	args.file_count = 0;

	for (int i = 1; i < argc; ++i)
	{
		if (*argv[i] == '-' && len(argv[i]) > 1)
		{
			if (!cmp_str(argv[i], "--"))
			{
				// Copy the rest of arguments in the file list
				for (int j = i; ++j < argc;)
					args.files[args.file_count++] = argv[j];
				break ;
			}
			// Parse the argument
			args.flags |= unwrap(parse_flag(argv[i]));
		}
		else
		{
			// Save the argument in the file list
			args.files[args.file_count++] = argv[i];
		}
	}

	// If no files are specified take the a.out
	if (args.file_count == 0)
		args.files[args.file_count++] = "a.out";
	// Terminate the char **
	args.files[args.file_count] = NULL;
	return Ok(args);
}

void	free_arguments(args_t *args)
{
	free(args->files);
}