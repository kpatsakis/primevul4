ut64 Elf_(r_bin_elf_p2v) (ELFOBJ *bin, ut64 paddr) {
	int i;

	if (!bin) return 0;

	if (!bin->phdr) {
		if (bin->ehdr.e_type == ET_REL) {
			return bin->baddr + paddr;
		}
		return paddr;
	}
	for (i = 0; i < bin->ehdr.e_phnum; ++i) {
		Elf_(Phdr) *p = &bin->phdr[i];
		if (!p) {
			break;
		}
		if (p->p_type == PT_LOAD && is_in_pphdr (p, paddr)) {
			if (!p->p_vaddr && !p->p_offset) {
				continue;
			}
			return p->p_vaddr + paddr - p->p_offset;
		}
	}

	return paddr;
}
