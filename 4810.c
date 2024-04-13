regname_internal_riscv (unsigned int regno)
{
  const char *name = NULL;

  /* Lookup in the table first, this covers GPR and FPR.  */
  if (regno < ARRAY_SIZE (dwarf_regnames_riscv))
    name = dwarf_regnames_riscv [regno];
  else if (regno >= 4096 && regno <= 8191)
    {
      /* This might be a CSR, these live in a sparse number space from 4096
	 to 8191  These numbers are defined in the RISC-V ELF ABI
	 document.  */
      switch (regno)
	{
#define DECLARE_CSR(NAME,VALUE,CLASS,DEFINE_VER,ABORT_VER) \
  case VALUE + 4096: name = #NAME; break;
#include "opcode/riscv-opc.h"
#undef DECLARE_CSR

	default:
	  {
	    static char csr_name[10];
	    snprintf (csr_name, sizeof (csr_name), "csr%d", (regno - 4096));
	    name = csr_name;
	  }
	  break;
	}
    }

  return name;
}