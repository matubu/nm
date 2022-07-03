#include <errno.h>
#include <string.h>

#include "io.h"

void	err(char *file, char *err)
{
	printf("nm: \033[91merror:\033[0m %s: %s\n", file, err);
}

void	sys_err(char *file)
{
	printf("nm: \033[91merror:\033[0m %s: %s\n", file, strerror(errno));
}
