int Elf_(r_bin_elf_get_static)(ELFOBJ *bin) {
	int i;
	if (!bin->phdr) {
		return false;
	}
	for (i = 0; i < bin->ehdr.e_phnum; i++) {
		if (bin->phdr[i].p_type == PT_INTERP) {
			return false;
		}
	}
	return true;
}
