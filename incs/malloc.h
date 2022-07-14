#pragma once

#include "io.h"
#include <stdlib.h>

static inline void	*_nm_malloc(size_t n) {
	void	*ptr = malloc(n);
	if (ptr == NULL)
	{
		sys_err("could not allocate memory");
		exit(1);
	}
	return (ptr);
}

#define malloc(n) _nm_malloc(n)