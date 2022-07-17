#pragma once

#include "int.h"
#include "res.h"

typedef struct {
	char	*path;
	byte	*ptr;
	usize	len;
}	file_t;

typedef file_t	*file_ptr;
NewResType(file_ptr);

void			free_file(file_t *s);
Res(file_ptr)	read_file(char *path);
