#include "elf/fields.h"

const char	*elf_class[] = {
	NULL,
	"32 bit",
	"64 bit"
};

const char	*elf_data[] = {
	NULL,
	"Little-endian",
	"Big-endian"
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

const char	*elf_type[] = {
	"Unknown",
	"Relocatable file",
	"Executable file",
	"Shared object",
	"Core file "
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