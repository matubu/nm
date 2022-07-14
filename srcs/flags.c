#include "flags.h"
#include "malloc.h"

flags_e	flags_mapping[256] = {
	['a'] = debugger_symbols,
	['g'] = only_global_symbols,
	['u'] = only_undefined_symbols,
	['r'] = reverse_sort,
	['p'] = no_sort
};

flags_e	parse_flag(char *s)
{
	flags_e	flag = 0;
	while (*++s)
	{
		if (flags_mapping[(uint)*s] == 0)
		{
			err("invalid option -- '%c'", *s);
			help();
			exit(1);
		}
		else
			flag |= flags_mapping[(uint)*s];
	}
	return (flag);
}

args_t	*parse_arguments(int argc, char **argv)
{
	args_t	*args = malloc(sizeof(args_t));

	args->flags = 0;
	args->files = malloc((argc < 2 ? 2 : argc) * sizeof(char *));
	args->file_count = 0;

	for (int i = 1; i < argc; ++i)
	{
		if (*argv[i] == '-')
		{
			if (argv[i][1] == '-' && argv[i][2] == '\0')
			{
				// Copy the rest of arguments in the file list
				for (int j = i; ++j < argc;)
					args->files[args->file_count++] = argv[j];
				break ;
			}
			// Parse the argument
			args->flags |= parse_flag(argv[i]);
		}
		else
		{
			// Save the argument in the file list
			args->files[args->file_count++] = argv[i];
		}
	}

	// If no files are specified take the a.out
	if (args->file_count == 0)
		args->files[args->file_count++] = "a.out";
	// Terminate the char **
	args->files[args->file_count] = NULL;
	return (args);
}

void	free_arguments(args_t *args)
{
	free(args->files);
	free(args);
}