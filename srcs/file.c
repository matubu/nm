#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "malloc.h"
#include "file.h"

#include <stdio.h>

static inline Res(file_ptr)	new_file_t(char *path, byte *ptr, usize len)
{
	Returns(file_ptr);

	file_ptr	f = malloc(sizeof(file_t));

	f->path = path;
	f->ptr = ptr;
	f->len = len;
	return Ok(f);
}

void	free_file(file_t *s)
{
	if (s == NULL)
		return ;
	munmap(s->ptr, s->len);
	free(s);
}

Res(file_ptr)	read_file(char *path)
{
	Returns(file_ptr);

	int	fd = open(path, O_RDONLY);
	struct stat info;

	if (fd < 0 || fstat(fd, &info) < 0)
		throw(strerror(errno));

	if (S_ISDIR(info.st_mode))
	{
		errno = EISDIR;
		throw(strerror(errno));
	}

	void	*ptr = mmap(NULL, info.st_size + 1, PROT_READ | PROT_WRITE, MAP_FILE | MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED)
		throw(strerror(errno));

	// put a \0 by security
	((char *)ptr)[info.st_size] = '\0';
	return (new_file_t(path, ptr, info.st_size));
}