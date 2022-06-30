#pragma once

#include "int.h"

typedef struct {
	char	*path;
	byte	*ptr;
	size_t	len;
}	file_t;

void	free_file(file_t *s);
file_t	*read_file(char *path);
int	starts_with(const file_t *f, byte *p);