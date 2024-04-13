ut64 Elf_(r_bin_elf_get_baddr)(ELFOBJ *bin) {
	int i;
	ut64 tmp, base = UT64_MAX;
	if (!bin) {
		return 0;
	}
	if (bin->phdr) {
		for (i = 0; i < bin->ehdr.e_phnum; i++) {
			if (bin->phdr[i].p_type == PT_LOAD) {
				tmp = (ut64)bin->phdr[i].p_vaddr & ELF_PAGE_MASK;
				tmp = tmp - (tmp % (1 << ELF_PAGE_SIZE));
				if (tmp < base) {
					base = tmp;
				}
			}
		}
	}
	if (base == UT64_MAX && bin->ehdr.e_type == ET_REL) {
		return 0x08000000;
	}
	return base == UT64_MAX ? 0 : base;
}
