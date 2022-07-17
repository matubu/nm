#pragma once

#include "io.h"
#include "res.h"
#include <stdlib.h>

typedef void	*void_ptr;
NewResType(void_ptr);

static inline Res(void_ptr)	_malloc(size_t n) {
	Returns(void_ptr);

	void	*ptr = malloc(n);
	if (ptr == NULL)
		throw("allocation error");
	return Ok(ptr);
}

#define malloc(n) unwrap(_malloc(n))