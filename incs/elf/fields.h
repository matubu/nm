#pragma once

#include <stdlib.h>

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

#define LittleEndian 1
#define BigEndian 2
#define CurrentEndian (2 - *(char *)((int []){1}) == 1)

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

#define ELF_SHENTSIZE ((elf_field_t) { \
	{0x2E, 0x3A}, \
	{2, 2} \
}) // Size of a section header table entry

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

#define SEC_NAME ((elf_field_t) { \
	{0x0, 0x0}, \
	{4, 4} \
}) // An offset to a string in the .shstrtab section that represents the name of this section

#define SEC_TYPE ((elf_field_t) { \
	{0x4, 0x4}, \
	{4, 4} \
}) // Identifies the type of this header
extern const char	*sec_type[];
#define SymbolTable              2
#define SymbolHashTable          5
#define DynamicLinkerSymbolTable 11

#define SEC_FLAGS ((elf_field_t) { \
	{0x8, 0x8}, \
	{4, 8} \
}) // Identifies the attributes of the section
extern const mask_mapping_t	sec_flags[];

#define SEC_ADDR ((elf_field_t) { \
	{0xC, 0x10}, \
	{4, 8} \
}) // Virtual address of the section in memory, for sections that are loaded

#define SEC_OFFSET ((elf_field_t) { \
	{0x10, 0x18}, \
	{4, 8} \
}) // Offset of the section in the file image

#define SEC_SIZE ((elf_field_t) { \
	{0x14, 0x20}, \
	{4, 8} \
}) // Size in bytes of the section in the file image. May be 0

#define SEC_LINK ((elf_field_t) { \
	{0x18, 0x28}, \
	{4, 4} \
}) // Contains the section index of an associated section. This field is used for several purposes, depending on the type of section

#define SEC_INFO ((elf_field_t) { \
	{0x1C, 0x2C}, \
	{4, 4} \
}) // Contains extra information about the section. This field is used for several purposes, depending on the type of section. 

#define SEC_ADDRALIGN ((elf_field_t) { \
	{0x20, 0x30}, \
	{4, 8} \
}) // Contains the required alignment of the section. This field must be a power of two. 

#define SEC_ENTSIZE ((elf_field_t) { \
	{0x24, 0x38}, \
	{4, 8} \
}) // Contains the size, in bytes, of each entry, for sections that contain fixed-size entries. Otherwise, this field contains zero. 

#define SEC_HEADER ((elf_field_t) { \
	{0, 0}, \
	{0x28, 0x40} \
}) // The start to end of the section header



// Symbol table header
// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
// https://blog.k3170makan.com/2018/10/introduction-to-elf-format-part-vi.html

#define SYM_NAME ((elf_field_t) { \
	{0x0, 0x0}, \
	{4, 4} \
}) // Offset string in string table can be NULL

#define SYM_VALUE ((elf_field_t) { \
	{0x4, 0x8}, \
	{4, 8} \
}) // Can be an absolute value, an address, and so forth. See https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblj/index.html#chapter6-35166

#define SYM_SIZE ((elf_field_t) { \
	{0x8, 0x10}, \
	{4, 8} \
}) // Can be the number of bytes contained in the object. If this member holds 0 the symbol has no size or an unknown size

#define SYM_INFO ((elf_field_t) { \
	{0xC, 0x4}, \
	{1, 1} \
}) // The symbol's type and binding attributes

// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
// https://blog.k3170makan.com/2018/10/introduction-to-elf-format-part-vi.html
#define ST_BIND(info) ((info) >> 4)
#define STB_LOCAL	0
#define STB_GLOBAL	1
#define STB_WEAK	2


#define ST_TYPE(info) ((info) & 0xf)

// https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
#define STT_NOTYPE			0 // type not defined
#define STT_OBJECT			1 // data object (variables, arrays...)
#define STT_FUNC			2 // function
#define STT_SECTION			3 // section
#define STT_FILE			4 // file
#define STT_COMMON			5 // common block (same as STT_OBJECT)
#define STT_TLS				6 // thread local entity

#define SYM_OTHER ((elf_field_t) { \
	{0xD, 0x5}, \
	{1, 1} \
}) // The symbol's visibility (mask 0x3)

#define SYM_SHNDX ((elf_field_t) { \
	{0xE, 0x6}, \
	{2, 4} \
}) // This member holds the relevant section header table index

#define SHN_UNDEF 0
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2
#define SHN_XINDEX 0xffff
