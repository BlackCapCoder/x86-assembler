#include <elf.h>
#include <cstdio>
#include <string.h>

void test ()
{
  Elf64_Ehdr header {};
  header.e_ident[EI_MAG0]       = ELFMAG0;
  header.e_ident[EI_MAG1]       = ELFMAG1;
  header.e_ident[EI_MAG2]       = ELFMAG2;
  header.e_ident[EI_MAG3]       = ELFMAG3;
  header.e_ident[EI_CLASS]      = ELFCLASS64;
  header.e_ident[EI_DATA]       = ELFDATA2LSB; // little endian
  header.e_ident[EI_VERSION]    = EV_CURRENT;
  header.e_ident[EI_OSABI]      = ELFOSABI_LINUX;
  header.e_ident[EI_ABIVERSION] = 0;
  memset(header.e_ident + EI_PAD, 0, EI_NIDENT - EI_PAD);

  header.e_type      = ET_EXEC;
  header.e_machine   = EM_X86_64;
  header.e_version   = EV_CURRENT;
  header.e_entry     = sizeof(decltype(header)); // entry point
  header.e_phoff     = 0; // program header table's file offset
  header.e_shoff     = 0; // section header table's file offset
  header.e_flags     = 0;
  header.e_ehsize    = sizeof(decltype(header));
  header.e_phentsize = 0; // size in bytes of one entry in the file's program header table; all entries are the same size
  header.e_phnum     = 0; // number of entries in the program header table
  header.e_shentsize = 0; // sections header's size in bytes
  header.e_shnum     = 0; // number of entries in the section header table
  header.e_shstrndx  = SHN_UNDEF; // section header table index of the entry associated with the section name string table

  auto file = fopen("elfout", "w");
  fwrite(&header, 1, sizeof(decltype(header)), file);
  fputc(0xc3, file);
  fclose(file);
}

int main (int argc, const char *argv[]) {
  test ();
  return 0;
}
