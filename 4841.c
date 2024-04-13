init_dwarf_regnames_by_bfd_arch_and_mach (enum bfd_architecture arch,
					  unsigned long mach)
{
  dwarf_regnames_lookup_func = NULL;

  switch (arch)
    {
    case bfd_arch_i386:
      switch (mach)
	{
	case bfd_mach_x86_64:
	case bfd_mach_x86_64_intel_syntax:
	case bfd_mach_x64_32:
	case bfd_mach_x64_32_intel_syntax:
	  init_dwarf_regnames_x86_64 ();
	  break;

	default:
	  init_dwarf_regnames_i386 ();
	  break;
	}
      break;

    case bfd_arch_iamcu:
      init_dwarf_regnames_iamcu ();
      break;

    case bfd_arch_aarch64:
      init_dwarf_regnames_aarch64();
      break;

    case bfd_arch_s390:
      init_dwarf_regnames_s390 ();
      break;

    case bfd_arch_riscv:
      init_dwarf_regnames_riscv ();
      break;

    default:
      break;
    }
}