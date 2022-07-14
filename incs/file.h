#pragma once

#include "int.h"

typedef struct {
	char	*path;
	byte	*ptr;
	usize	len;
}	file_t;

void	free_file(file_t *s);
file_t	*read_file(char *path);