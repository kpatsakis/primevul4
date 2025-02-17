int Elf_(r_bin_elf_get_bits)(ELFOBJ *bin) {
	/* Hack for ARCompact */
	if (bin->ehdr.e_machine == EM_ARC_A5) {
		return 16;
	}
	/* Hack for Ps2 */
	if (bin->phdr && bin->ehdr.e_machine == EM_MIPS) {
		const ut32 mipsType = bin->ehdr.e_flags & EF_MIPS_ARCH;
		if (bin->ehdr.e_type == ET_EXEC) {
			int i;
			bool haveInterp = false;
			for (i = 0; i < bin->ehdr.e_phnum; i++) {
				if (bin->phdr[i].p_type == PT_INTERP) {
					haveInterp = true;
				}
			}
			if (!haveInterp && mipsType == EF_MIPS_ARCH_3) {
				return 64;
			}
		}
		switch (mipsType) {
		case EF_MIPS_ARCH_1:
		case EF_MIPS_ARCH_2:
		case EF_MIPS_ARCH_3:
		case EF_MIPS_ARCH_4:
		case EF_MIPS_ARCH_5:
		case EF_MIPS_ARCH_32:
			return 32;
		case EF_MIPS_ARCH_64:
			return 64;
		case EF_MIPS_ARCH_32R2:
			return 32;
		case EF_MIPS_ARCH_64R2:
			return 64;
			break;
		}
		return 32;
	}
	/* Hack for Thumb */
	if (bin->ehdr.e_machine == EM_ARM) {
		if (bin->ehdr.e_type != ET_EXEC) {
			struct r_bin_elf_symbol_t *symbol;
			if ((symbol = Elf_(r_bin_elf_get_symbols) (bin))) {
				int i = 0;
				for (i = 0; !symbol[i].last; i++) {
					ut64 paddr = symbol[i].offset;
					if (paddr & 1) {
						return 16;
					}
				}
			}
		}
		{
			ut64 entry = Elf_(r_bin_elf_get_entry_offset) (bin);
			if (entry & 1) {
				return 16;
			}
		}
	}
	switch (bin->ehdr.e_ident[EI_CLASS]) {
	case ELFCLASS32:   return 32;
	case ELFCLASS64:   return 64;
	case ELFCLASSNONE:
	default:           return 32; // defaults
	}
}
