#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "file.h"


#include <stdio.h>

static inline file_t	*new_file_t(char *path, byte *ptr, usize len)
{
	file_t	*f = malloc(sizeof(file_t));
	if (f == NULL)
		return (NULL);

	f->path = path;
	f->ptr = ptr;
	f->len = len;
	return (f);
}

void	free_file(file_t *s)
{
	munmap(s->ptr, s->len);
	free(s);
}

file_t	*read_file(char *path)
{
	int	fd = open(path, O_RDONLY);
	struct stat info;

	if (fd < 0)
		return (NULL);
	if (fstat(fd, &info) < 0)
		return (NULL);
	if (S_ISDIR(info.st_mode))
	{
		errno = EISDIR;
		return (NULL);
	}

	void	*ptr = mmap(NULL, info.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED)
		return NULL;

	return (new_file_t(path, ptr, info.st_size));
}

int	starts_with(const file_t *f, byte *p)
{
	printf("starts_with(%x, %x)\n", *f->ptr, *p);
	for (usize i = 0; p[i]; ++i)
		if (i >= f->len || p[i] != f->ptr[i])
			return (0);
	return (1);
}