#pragma once

#include <string.h>
#include "int.h"

static inline u64	len(char *s) {
	u64	len = 0;

	while (*s)
	{
		++s;
		++len;
	}
	return (len);
}

static inline i64		cmp_bytes(const byte *a, const byte *b) {
	if (a == NULL || b == NULL)
		return (a - b);

	while (*a != '\0' && *a == *b)
	{
		++a;
		++b;
	}
	return (*a - *b);
}

static inline i64		cmp_str(const char *a, const char *b) {
	return (cmp_bytes((byte *)a, (byte *)b));
}