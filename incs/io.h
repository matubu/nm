#pragma once

#include <stdio.h>
#include <unistd.h>

#define PUTS(fd, s) write(fd, s, sizeof(s) - 1);
void	err(char *file, char *err);
void	sys_err(char *file);