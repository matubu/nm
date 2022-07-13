#pragma once

#include <stdio.h>
#include <unistd.h>

// https://stackoverflow.com/questions/11888594/ignoring-return-values-in-c
// Fix compilation error because of ignored return value (ex: write in PUTS macro)
#define ignore(F) if (F){}

#define PUTS(fd, s) ignore(write(fd, s, sizeof(s) - 1));
void	err(char *file, char *err);
void	sys_err(char *file);