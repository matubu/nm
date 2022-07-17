#pragma once

#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include "str.h"

// https://stackoverflow.com/questions/11888594/ignoring-return-values-in-c
// Fix compilation error because of ignored return value (ex: write in PUTS macro)
#define ignore(F) if (F){}

#define PUTS(fd, s) ignore(write(fd, s "\n", sizeof(s)));

void	fd_puts(int fd, char *s);
void	fd_fmt(int fd, char *fmt, ...);
void	help();

#define fmt(...) fd_fmt(1, __VA_ARGS__)

#define err(fmt, ...) { \
	fd_fmt(2, "nm: \033[91merror:\033[0m " fmt "\n", __VA_ARGS__); \
}
#define sys_err(fmt, ...) { \
	err(fmt ": %s", ##__VA_ARGS__, strerror(errno)); \
}
