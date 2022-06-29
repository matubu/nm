// This file will be compiled to generate a elf for testing

#include <stdio.h>

int	add(int a, int b)
{
	return (a + b);
}

int	main()
{
	printf("%d\n", add(5, 5));
}