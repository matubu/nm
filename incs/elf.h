#pragma once

#include <stdlib.h>

typedef unsigned char	byte;
typedef unsigned long	size_t;

typedef struct {
	size_t	off[2];  // [ off32 , off64  ]
	size_t	size[2]; // [ size32, size64 ]
}	elf_field_t;

typedef struct {
	uint64_t	mask;
	char		*mapped;
}	mask_mapping_t;

#define GET_STRING_MAPPING(arr, n) n >= (sizeof(arr) / sizeof(arr[0])) ? NULL : arr[n]



// File header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header

elf_field_t ELF_MAGIC = {
	{0x0, 0x0},
	{4, 4}
};
#define elf_magic (*(uint64_t *)("\x7F""ELF\0\0\0\0"))

elf_field_t ELF_CLASS = {
	{0x4, 0x4},
	{1, 1}
};
const char	*elf_class[] = {
	NULL,
	"32 bit",
	"64 bit"
};

elf_field_t ELF_DATA = {
	{0x5, 0x5},
	{1, 1}
};
const char	*elf_data[] = {
	NULL,
	"Little-endian",
	"Big-endian"
};

elf_field_t ELF_EI_VERSION = {
	{0x6, 0x6},
	{1, 1}
}; // 1 = original and current version of ELF

elf_field_t ELF_OSABI = {
	{0x7, 0x7},
	{1, 1}
};
const char	*elf_osabi[] = {
	"System V",
	"HP-UX",
	"NetBSD",
	"Linux",
	"GNU Hurd",
	"Solaris",
	"AIX (Monterey)",
	"IRIX",
	"FreeBSD",
	"Tru64",
	"Novell Modesto",
	"OpenBSD",
	"OpenVMS",
	"NonStop Kernel",
	"AROS",
	"FenixOS",
	"Nuxi CloudABI",
	"Stratus Technologies OpenVOS"
};

elf_field_t ELF_ABIVERSION = {
	{0x8, 0x8},
	{1, 1}
};
elf_field_t ELF_PAD      = {
	{0x9, 0x9},
	{7, 7}
};

elf_field_t ELF_TYPE     = {
	{0x10, 0x10},
	{2, 2}
};
const char	*elf_type[] = {
	"Unknown",
	"Relocatable file",
	"Executable file",
	"Shared object",
	"Core file "
};

elf_field_t ELF_MACHINE  = {
	{0x12, 0x12},
	{2, 2}
};
const char	*elf_machine[] = {
	[0x00]="No specific instruction set",
	[0x01]="AT&T WE 32100",
	[0x02]="SPARC",
	[0x03]="x86",
	[0x04]="Motorola 68000 (M68k)",
	[0x05]="Motorola 88000 (M88k)",
	[0x06]="Intel MCU",
	[0x07]="Intel 80860",
	[0x08]="MIPS",
	[0x09]="IBM System/370",
	[0x0A]="MIPS RS3000 Little-endian",
	[0x0E]="Hewlett-Packard PA-RISC",
	[0x0F]="Reserved for future use",
	[0x13]="Intel 80960",
	[0x14]="PowerPC",
	[0x15]="PowerPC (64-bit)",
	[0x16]="S390, including S390x",
	[0x17]="IBM SPU/SPC",
	[0x24]="NEC V800",
	[0x25]="Fujitsu FR20",
	[0x26]="TRW RH-32",
	[0x27]="Motorola RCE",
	[0x28]="ARM (up to ARMv7/Aarch32)",
	[0x29]="Digital Alpha",
	[0x2A]="SuperH",
	[0x2B]="SPARC Version 9",
	[0x2C]="Siemens TriCore embedded processor",
	[0x2D]="Argonaut RISC Core",
	[0x2E]="Hitachi H8/300",
	[0x2F]="Hitachi H8/300H",
	[0x30]="Hitachi H8S",
	[0x31]="Hitachi H8/500",
	[0x32]="IA-64",
	[0x33]="Stanford MIPS-X",
	[0x34]="Motorola ColdFire",
	[0x35]="Motorola M68HC12",
	[0x36]="Fujitsu MMA Multimedia Accelerator",
	[0x37]="Siemens PCP",
	[0x38]="Sony nCPU embedded RISC processor",
	[0x39]="Denso NDR1 microprocessor",
	[0x3A]="Motorola Star*Core processor",
	[0x3B]="Toyota ME16 processor",
	[0x3C]="STMicroelectronics ST100 processor",
	[0x3D]="Advanced Logic Corp. TinyJ embedded processor family",
	[0x3E]="AMD x86-64",
	[0x3F]="Sony DSP Processor",
	[0x40]="Digital Equipment Corp. PDP-10",
	[0x41]="Digital Equipment Corp. PDP-11",
	[0x42]="Siemens FX66 microcontroller",
	[0x43]="STMicroelectronics ST9+ 8/16 bit microcontroller",
	[0x44]="STMicroelectronics ST7 8-bit microcontroller",
	[0x45]="Motorola MC68HC16 Microcontroller",
	[0x46]="Motorola MC68HC11 Microcontroller",
	[0x47]="Motorola MC68HC08 Microcontroller",
	[0x48]="Motorola MC68HC05 Microcontroller",
	[0x49]="Silicon Graphics SVx",
	[0x4A]="STMicroelectronics ST19 8-bit microcontroller",
	[0x4B]="Digital VAX",
	[0x4C]="Axis Communications 32-bit embedded processor",
	[0x4D]="Infineon Technologies 32-bit embedded processor",
	[0x4E]="Element 14 64-bit DSP Processor",
	[0x4F]="LSI Logic 16-bit DSP Processor",
	[0x8C]="TMS320C6000 Family",
	[0xAF]="MCST Elbrus e2k",
	[0xB7]="ARM 64-bits (ARMv8/Aarch64)",
	[0xDC]="Zilog_Z80",
	[0xF3]="RISC-V",
	[0xF7]="Berkeley Packet Filter",
	[0x101]="WDC 65C816"
};

elf_field_t ELF_VERSION = {
	{0x14, 0x14},
	{4, 4}
}; // 1 = original version of ELF

elf_field_t ELF_ENTRY = {
	{0x18, 0x18},
	{4, 8}
}; // Address of the entry point

elf_field_t ELF_PHOFF = {
	{0x1C, 0x20},
	{4, 8}
}; // Address of the program header table

elf_field_t ELF_SHOFF = {
	{0x20, 0x28},
	{4, 8}
}; // Address of the section header table

elf_field_t ELF_FLAGS = {
	{0x24, 0x30},
	{4, 4}
}; // Interpretation depends on the target architecture

elf_field_t ELF_EHSIZE = {
	{0x28, 0x34},
	{2, 2}
}; // Size of this header

elf_field_t ELF_PHENTSIZE = {
	{0x2A, 0x36},
	{2, 2}
}; // Size of a program header table entry

elf_field_t ELF_PHNUM = {
	{0x2C, 0x38},
	{2, 2}
}; // The number of entries in the program header table

elf_field_t ELF_SHENTSIZE = {
	{0x2E, 0x3A},
	{2, 2}
}; // Size of a section header table entry

elf_field_t ELF_SHNUM = {
	{0x30, 0x3C},
	{2, 2}
}; // Number of entries in the section header table

elf_field_t ELF_SHSTRNDX = {
	{0x32, 0x3E},
	{2, 2}
}; // Index of the section header table entry that contains the section names

elf_field_t ELF_HEADER = {
	{0, 0},
	{0x34, 0x40}
}; // The start to end of the file header



// Program header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#Program_header

elf_field_t PROG_TYPE = {
	{0x0, 0x0},
	{4, 4}
}; // Identifies the type of the segment
const char	*prog_type[] = {
	"Program header table entry unused",
	"Loadable segment",
	"Dynamic linking information",
	"Interpreter information",
	"Auxiliary information",
	"Reserved",
	"Segment containing program header table itself",
	"Thread-Local Storage template"
};

elf_field_t PROG_FLAGS = {
	{0x18, 0x4},
	{4, 4}
}; // Segment-dependent flags

elf_field_t PROG_OFFSET = {
	{0x4, 0x8},
	{4, 8}
}; // Offset of the segment in the file image

elf_field_t PROG_VADDR = {
	{0x8, 0x10},
	{4, 8}
}; // Virtual address of the segment in memory

elf_field_t PROG_PADDR = {
	{0x0C, 0x18},
	{4, 8}
}; // On systems where physical address is relevant, reserved for segment's physical address

elf_field_t PROG_FILESZ = {
	{0x10, 0x20},
	{4, 8}
}; // Size in bytes of the segment in the file image. May be 0

elf_field_t PROG_MEMSZ = {
	{0x14, 0x28},
	{4, 8}
}; // Size in bytes of the segment in memory. May be 0

elf_field_t PROG_ALIGN = {
	{0x1C, 0x30},
	{4, 8}
}; // 0 and 1 specify no alignment. Otherwise should be a positive, integral power of 2, with p_vaddr equating p_offset modulus p_align

elf_field_t PROG_HEADER = {
	{0, 0},
	{0x20, 0x38}
}; // The start to end of the program header



// Section header
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#Section_header

elf_field_t SEC_NAME = {
	{0x0, 0x0},
	{4, 4}
}; // An offset to a string in the .shstrtab section that represents the name of this section

elf_field_t SEC_TYPE = {
	{0x4, 0x4},
	{4, 4}
}; // Identifies the type of this header
const char	*sec_type[] = {
	"Section header table entry unused",
	"Program data",
	"Symbol table",
	"String table",
	"Relocation entries with addends",
	"Symbol hash table",
	"Dynamic linking information",
	"Notes",
	"Program space with no data (bss)",
	"Relocation entries, no addends",
	"Reserved",
	"Dynamic linker symbol table",
	"Array of constructors",
	"Array of destructors",
	"Array of pre-constructors",
	"Section group",
	"Extended section indices",
	"Number of defined types"
};

elf_field_t SEC_FLAGS = {
	{0x8, 0x8},
	{4, 8}
}; // Identifies the attributes of the section
const mask_mapping_t	sec_flags[] = {
	// [mask-shift] = mapping
	{0x1, "Writable"},
	{0x2, "Occupies memory during execution"},
	{0x4, "Executable"},
	{0x10, "Might be merged"},
	{0x20, "Contains null-terminated strings"},
	{0x40, "'sh_info' contains SHT index"},
	{0x80, "Preserve order after combining"},
	{0x100, "Non-standard OS specific handling required"},
	{0x200, "Section is member of a group"},
	{0x400, "Section hold thread-local data"},
	{0x4000000, "Special ordering requirement (Solaris)"},
	{0x8000000, "Section is excluded unless referenced or allocated (Solaris)"},
	{0, NULL}
};

elf_field_t SEC_ADDR = {
	{0xC, 0x10},
	{4, 8}
}; // Virtual address of the section in memory, for sections that are loaded

elf_field_t SEC_OFFSET = {
	{0x10, 0x18},
	{4, 8}
}; // Offset of the section in the file image

elf_field_t SEC_SIZE = {
	{0x14, 0x20},
	{4, 8}
}; // Size in bytes of the section in the file image. May be 0

elf_field_t SEC_LINK = {
	{0x18, 0x28},
	{4, 4}
}; // Contains the section index of an associated section. This field is used for several purposes, depending on the type of section

elf_field_t SEC_INFO = {
	{0x1C, 0x2C},
	{4, 4}
}; // Contains extra information about the section. This field is used for several purposes, depending on the type of section. 

elf_field_t SEC_ADDRALIGN = {
	{0x20, 0x30},
	{4, 8}
}; // Contains the required alignment of the section. This field must be a power of two. 

elf_field_t SEC_ENTSIZE = {
	{0x24, 0x38},
	{4, 8}
}; // Contains the size, in bytes, of each entry, for sections that contain fixed-size entries. Otherwise, this field contains zero. 

elf_field_t SEC_HEADER = {
	{0, 0},
	{0x28, 0x40}
}; // The start to end of the section header



typedef struct {
	char	*path;
	byte	*ptr;
	size_t	len;
}	file_t;

typedef struct {
	char	*prog_header;
	char	*code;
	char	*data;
	char	*sections_name;
	char	*sec_header;
}	elf_mapping_t;

typedef struct {
	const file_t	*f;
	int				class; // 1 == 32bit,  2 == 64bit
	int				endian; // 1 == Little-endian,  2 == Big-endian
	elf_mapping_t	mapping;
}	elf_t;
