init_dwarf_regnames_by_elf_machine_code (unsigned int e_machine)
{
  dwarf_regnames_lookup_func = NULL;

  switch (e_machine)
    {
    case EM_386:
      init_dwarf_regnames_i386 ();
      break;

    case EM_IAMCU:
      init_dwarf_regnames_iamcu ();
      break;

    case EM_X86_64:
    case EM_L1OM:
    case EM_K1OM:
      init_dwarf_regnames_x86_64 ();
      break;

    case EM_AARCH64:
      init_dwarf_regnames_aarch64 ();
      break;

    case EM_S390:
      init_dwarf_regnames_s390 ();
      break;

    case EM_RISCV:
      init_dwarf_regnames_riscv ();
      break;

    default:
      break;
    }
}