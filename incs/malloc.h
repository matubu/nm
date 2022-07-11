#pragma once

#include "io.h"
#include <stdlib.h>

static inline void	*_nm_malloc(size_t n) {
	void	*ptr = malloc(n);
	if (ptr == NULL)
	{
		PUTS(2, "nm: \033[91merror:\033[0m: could not allocate memory\n");
		exit(1);
	}
	return (ptr);
}

#define malloc(n) _nm_malloc(n)