int Elf_(r_bin_elf_has_nx)(ELFOBJ *bin) {
	int i;
	if (bin && bin->phdr) {
		for (i = 0; i < bin->ehdr.e_phnum; i++) {
			if (bin->phdr[i].p_type == PT_GNU_STACK) {
				return (!(bin->phdr[i].p_flags & 1))? 1: 0;
			}
		}
	}
	return 0;
}
