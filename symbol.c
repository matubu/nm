#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
	uint32_t      st_name;
	uint32_t      st_value;
	uint32_t      st_size;
	unsigned char   st_info;
	unsigned char   st_other;
	uint16_t      st_shndx;
} Elf32_Sym;

typedef struct {
	uint32_t      st_name;
	unsigned char   st_info;
	unsigned char   st_other;
	uint16_t      st_shndx;
	uint64_t      st_value;
	uint64_t     st_size;
} Elf64_Sym;

int main()
{
	printf("32bit %lx\n", sizeof(Elf32_Sym));
	printf("64bit %lx\n", sizeof(Elf64_Sym));
	printf("st_name %lx\n", offsetof(Elf32_Sym, st_name));
	printf("st_value %lx\n", offsetof(Elf32_Sym, st_value));
	printf("st_size %lx\n", offsetof(Elf32_Sym, st_size));
	printf("st_info %lx\n", offsetof(Elf32_Sym, st_info));
	printf("st_other %lx\n", offsetof(Elf32_Sym, st_other));
	printf("st_shndx %lx\n", offsetof(Elf32_Sym, st_shndx));
	puts(")____(");
	printf("st_name %lx\n", offsetof(Elf64_Sym, st_name));
	printf("st_value %lx\n", offsetof(Elf64_Sym, st_value));
	printf("st_size %lx\n", offsetof(Elf64_Sym, st_size));
	printf("st_info %lx\n", offsetof(Elf64_Sym, st_info));
	printf("st_other %lx\n", offsetof(Elf64_Sym, st_other));
	printf("st_shndx %lx\n", offsetof(Elf64_Sym, st_shndx));
}