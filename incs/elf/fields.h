#pragma once

#include <stddef.h>

#include "malloc.h"
#include "int.h"

typedef struct {
	usize	off[2];  // [ off32 , off64  ]
	usize	size[2]; // [ size32, size64 ]
}	elf_field_t;

typedef struct {
	u64	mask;
	char		*mapped;
}	mask_mapping_t;

#define GET_STRING_MAPPING(arr, n) n >= (sizeof(arr) / sizeof(arr[0])) ? NULL : arr[n]

// Data Representation
// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter7-6.html#scrolltoc
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef uint64_t Elf64_Sxword;


// Macro to create elf_field_t data from struct
#define sizeofmember(type, member) (sizeof(((type*)0)->member))
#define DEFINE_FIELD(type32, type64, member) ((elf_field_t) { \
	{offsetof(type32, member), offsetof(type64, member)}, \
	{sizeofmember(type32, member), sizeofmember(type64, member)} \
})

// https://refspecs.linuxbase.org/elf/elf.pdf

// File header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header

#define ELF_MAGIC ((elf_field_t) { \
	{0x0, 0x0}, \
	{4, 4} \
})
#define elf_magic (*(u64 *)("\x7F""ELF\0\0\0\0"))

#define ELF_CLASS ((elf_field_t) { \
	{0x4, 0x4}, \
	{1, 1} \
})

#define LittleEndian 1
#define BigEndian 2
// https://www.embeddedsoft.net/simple-code-to-find-endianness-in-c/
#define CurrentEndian (2 - *(char *)((int []){1}) == 1)

extern const char	*elf_class[];

#define ELF_DATA ((elf_field_t) { \
	{0x5, 0x5}, \
	{1, 1} \
})
extern const char	*elf_data[];

#define ELF_EI_VERSION ((elf_field_t) { \
	{0x6, 0x6}, \
	{1, 1} \
}) // 1 = original and current version of ELF

#define ELF_OSABI ((elf_field_t) { \
	{0x7, 0x7}, \
	{1, 1} \
})
extern const char	*elf_osabi[];

#define ELF_ABIVERSION ((elf_field_t) { \
	{0x8, 0x8}, \
	{1, 1} \
})
#define ELF_PAD      ((elf_field_t) { \
	{0x9, 0x9}, \
	{7, 7} \
})

#define ELF_TYPE     ((elf_field_t) { \
	{0x10, 0x10}, \
	{2, 2} \
})
extern const char	*elf_type[];

#define ELF_MACHINE  ((elf_field_t) { \
	{0x12, 0x12}, \
	{2, 2} \
})
extern const char	*elf_machine[];

#define ELF_VERSION ((elf_field_t) { \
	{0x14, 0x14}, \
	{4, 4} \
}) // 1 = original version of ELF

#define ELF_ENTRY ((elf_field_t) { \
	{0x18, 0x18}, \
	{4, 8} \
}) // Address of the entry point

#define ELF_PHOFF ((elf_field_t) { \
	{0x1C, 0x20}, \
	{4, 8} \
}) // Address of the program header table

#define ELF_SHOFF ((elf_field_t) { \
	{0x20, 0x28}, \
	{4, 8} \
}) // Address of the section header table

#define ELF_FLAGS ((elf_field_t) { \
	{0x24, 0x30}, \
	{4, 4} \
}) // Interpretation depends on the target architecture

#define ELF_EHSIZE ((elf_field_t) { \
	{0x28, 0x34}, \
	{2, 2} \
}) // Size of this header

#define ELF_PHENTSIZE ((elf_field_t) { \
	{0x2A, 0x36}, \
	{2, 2} \
}) // Size of a program header table entry

#define ELF_PHNUM ((elf_field_t) { \
	{0x2C, 0x38}, \
	{2, 2} \
}) // The number of entries in the program header table

// standard name: e_shentsize
// Size of a section header table entry
#define ELF_SHENTSIZE ((elf_field_t) { \
	{0x2E, 0x3A}, \
	{2, 2} \
})

#define ELF_SHNUM ((elf_field_t) { \
	{0x30, 0x3C}, \
	{2, 2} \
}) // Number of entries in the section header table

#define ELF_SHSTRNDX ((elf_field_t) { \
	{0x32, 0x3E}, \
	{2, 2} \
}) // Index of the section header table entry that contains the section names

#define ELF_HEADER ((elf_field_t) { \
	{0, 0}, \
	{0x34, 0x40} \
}) // The start to end of the file header



// Program header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#Program_header

#define PROG_TYPE ((elf_field_t) { \
	{0x0, 0x0}, \
	{4, 4} \
}) // Identifies the type of the segment
extern const char	*prog_type[];

#define PROG_FLAGS ((elf_field_t) { \
	{0x18, 0x4}, \
	{4, 4} \
}) // Segment-dependent flags

#define PROG_OFFSET ((elf_field_t) { \
	{0x4, 0x8}, \
	{4, 8} \
}) // Offset of the segment in the file image

#define PROG_VADDR ((elf_field_t) { \
	{0x8, 0x10}, \
	{4, 8} \
}) // Virtual address of the segment in memory

#define PROG_PADDR ((elf_field_t) { \
	{0x0C, 0x18}, \
	{4, 8} \
}) // On systems where physical address is relevant, reserved for segment's physical address

#define PROG_FILESZ ((elf_field_t) { \
	{0x10, 0x20}, \
	{4, 8} \
}) // Size in bytes of the segment in the file image. May be 0

#define PROG_MEMSZ ((elf_field_t) { \
	{0x14, 0x28}, \
	{4, 8} \
}) // Size in bytes of the segment in memory. May be 0

#define PROG_ALIGN ((elf_field_t) { \
	{0x1C, 0x30}, \
	{4, 8} \
}) // 0 and 1 specify no alignment. Otherwise should be a positive, integral power of 2, with p_vaddr equating p_offset modulus p_align



// Section header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#Section_header

// standard name: sh_name
// An offset to a string in the .shstrtab section that represents the name of this section
#define SEC_NAME ((elf_field_t) { \
	{0x0, 0x0}, \
	{4, 4} \
})

// standard name: sh_type
// Identifies the type of this header
#define SEC_TYPE ((elf_field_t) { \
	{0x4, 0x4}, \
	{4, 4} \
})
extern const char	*sec_type[];
// https://docs.oracle.com/cd/E19683-01/817-3677/chapter6-94076/index.html
// standard name: SHT_
#define SEC_TYPE_NULL			0 // Identifies the section header as inactive
#define SEC_TYPE_PROGBITS		1 // Identifies information defined by the program
#define SEC_TYPE_SYMTAB			2 // Identifies a symbol table
#define SEC_TYPE_STRTAB			3 // Identifies a string table
#define SEC_TYPE_RELA			4 // Identifies relocation entries with explicit addends
#define SEC_TYPE_HASH			5 // Identifies a symbol hash table
#define SEC_TYPE_DYNAMIC		6 // Identifies information for dynamic linking
#define SEC_TYPE_NOTE			7 // Identifies information that marks the file in some way
#define SEC_TYPE_NOBITS			8 // Identifies a section that occupies no space in the file but otherwise resembles SHT_PROGBITS
#define SEC_TYPE_REL			9 // Identifies relocation entries without explicit addends
#define SEC_TYPE_SHLIB			10 // Identifies a reserved section which has unspecified semantics
#define SEC_TYPE_DYNSYM			11 // Identifies a section containing an array of pointers to initialization functions
#define SEC_TYPE_INIT_ARRAY		14 // Identifies a section containing an array of pointers to initialization functions
#define SEC_TYPE_FINI_ARRAY		15 // Identifies a section containing an array of pointers to termination functions
#define SEC_TYPE_PREINIT_ARRAY	16 // Identifies a section containing an array of pointers to functions that are invoked before all other initialization functions
#define SEC_TYPE_GROUP			17 // Identifies a section group
#define SEC_TYPE_SYMTAB_SHNDX	18 // Identifies a section containing extended section indexes

#define SEC_TYPE_SUNW_DEBUGSTR	0x6ffffff8 // Identifies debugging information
#define SEC_TYPE_SUNW_DEBUG		0x6ffffff9

#define SEC_TYPE_SUNW_move		0x6ffffffa // Identifies data to handle partially initialized symbols
#define SEC_TYPE_SUNW_COMDAT	0x6ffffffb // Identifies a section that allows multiple copies of the same data to be reduced to a single copy
#define SEC_TYPE_SUNW_syminfo	0x6ffffffc // Identifies additional symbol information
#define SEC_TYPE_SUNW_verdef	0x6ffffffd // Identifies fine-grained versions defined by this file
#define SEC_TYPE_SUNW_verneed	0x6ffffffe // Identifies fine-grained dependencies required by this file
#define SEC_TYPE_SUNW_versym	0x6fffffff // Identifies a table describing the relationship of symbols to the version definitions offered by the file

// standard name: sh_flags
// Identifies the attributes of the section
#define SEC_FLAGS ((elf_field_t) { \
	{0x8, 0x8}, \
	{4, 8} \
})
extern const mask_mapping_t	sec_flags[];
// https://docs.oracle.com/cd/E37838_01/html/E61063/elf-23207.html
// standard name: SHF_
#define SEC_FLAG_WRITE 0x1
#define SEC_FLAG_ALLOC 0x2
#define SEC_FLAG_EXECINSTR 0x4

// standard name: sh_addr
// Virtual address of the section in memory, for sections that are loaded
#define SEC_ADDR ((elf_field_t) { \
	{0xC, 0x10}, \
	{4, 8} \
})

// standard name: sh_offset
// Offset of the section in the file image
#define SEC_OFFSET ((elf_field_t) { \
	{0x10, 0x18}, \
	{4, 8} \
})

// standard name: sh_link
// Size in bytes of the section in the file image. May be 0
#define SEC_SIZE ((elf_field_t) { \
	{0x14, 0x20}, \
	{4, 8} \
})

// standard name: sh_link
// Contains the section index of an associated section.
// This field is used for several purposes, depending on the type of section
#define SEC_LINK ((elf_field_t) { \
	{0x18, 0x28}, \
	{4, 4} \
})

// standard name: sh_info
// Contains extra information about the section.
// This field is used for several purposes, depending on the type of section.
#define SEC_INFO ((elf_field_t) { \
	{0x1C, 0x2C}, \
	{4, 4} \
})

// standard name: sh_addralign
// Contains the required alignment of the section.
// This field must be a power of two.
#define SEC_ADDRALIGN ((elf_field_t) { \
	{0x20, 0x30}, \
	{4, 8} \
})

// standard name: sh_entsize
// Contains the size, in bytes, of each entry,
// for sections that contain fixed-size entries.
// Otherwise, this field contains zero.
#define SEC_ENTSIZE ((elf_field_t) { \
	{0x24, 0x38}, \
	{4, 8} \
})

// The start to end of the section header
#define SEC_HEADER ((elf_field_t) { \
	{0, 0}, \
	{0x28, 0x40} \
})



// Symbol table header
// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
// https://blog.k3170makan.com/2018/10/introduction-to-elf-format-part-vi.html

typedef struct {
	Elf32_Word      st_name;
	Elf32_Addr      st_value;
	Elf32_Word      st_size;
	unsigned char   st_info;
	unsigned char   st_other;
	Elf32_Half      st_shndx;
}	Elf32_Sym;

typedef struct {
	Elf64_Word      st_name;
	unsigned char   st_info;
	unsigned char   st_other;
	Elf64_Half      st_shndx;
	Elf64_Addr      st_value;
	Elf64_Xword     st_size;
}	Elf64_Sym;

// Offset string in string table can be NULL
#define SYM_NAME  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_name)

// Can be an absolute value, an address, and so forth. See https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblj/index.html#chapter6-35166
#define SYM_VALUE  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_value)

// Can be the number of bytes contained in the object. If this member holds 0 the symbol has no size or an unknown size
#define SYM_SIZE  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_size)

// The symbol's type and binding attributes
#define SYM_INFO  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_info)

// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
// https://blog.k3170makan.com/2018/10/introduction-to-elf-format-part-vi.html
// standard name: ST_BIND
#define SYM_BIND(info)		((info) >> 4)
// standard name: STB (symbol type bind)
#define SYM_BIND_LOCAL		0
#define SYM_BIND_GLOBAL		1
#define SYM_BIND_WEAK		2
// -fno-gnu-unique in https://gcc.gnu.org/onlinedocs/gcc-9.1.0/gcc/Code-Gen-Options.html
// value from: https://github.com/aligrudi/neatlibc/blob/master/elf.h
#define SYM_BIND_GNU_UNIQUE	10

// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
// standard name: ST_TYPE
#define SYM_TYPE(info)		((info) & 0xf)
// standard name: STT_ (symbol type type)
#define SYM_TYPE_NOTYPE		0 // type not defined
#define SYM_TYPE_OBJECT		1 // data object (variables, arrays...)
#define SYM_TYPE_FUNC		2 // function
#define SYM_TYPE_SECTION	3 // section
#define SYM_TYPE_FILE		4 // file
#define SYM_TYPE_COMMON		5 // common block (same as SYM_TYPE_OBJECT)
#define SYM_TYPE_TLS		6 // thread local entity


// The symbol's visibility (mask 0x3)
#define SYM_OTHER  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_other)

// standard name: st_shndx (section header index)
// This member holds the relevant section header table index
#define SYM_REL  DEFINE_FIELD(Elf32_Sym, Elf64_Sym, st_shndx)

// https://docs.oracle.com/cd/E19683-01/817-3677/chapter6-94076/index.html
// standard name: SHN_ (section header index)

// An undefined, missing, irrelevant, or otherwise meaningless section reference.
// For example, a symbol defined relative to section number SHN_UNDEF is an undefined symbol.
#define SYM_REL_UNDEF  0x0

// Absolute values for the corresponding reference.
// For example, symbols defined relative to section number SHN_ABS have absolute values and are not affected by relocation.
#define SYM_REL_ABS    0xfff1

// Symbols defined relative to this section are common symbols,
// such as FORTRAN COMMON or unallocated C external variables.
// These symbols are sometimes referred to as tentative
#define SYM_REL_COMMON 0xfff2

// An escape value indicating that the actual section header index is too large to fit in the containing field.
// The header section index is found in another location specific to the structure where it appears.
#define SYM_REL_XINDEX 0xffff


// The start to end of the symbol header
#define SYM_HEADER   ((elf_field_t) { \
	{0, 0}, \
	{sizeof(Elf32_Sym), sizeof(Elf64_Sym)} \
})
