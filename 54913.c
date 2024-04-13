ut64 Elf_(r_bin_elf_v2p) (ELFOBJ *bin, ut64 vaddr) {
	int i;
	if (!bin) {
		return 0;
	}
	if (!bin->phdr) {
		if (bin->ehdr.e_type == ET_REL) {
			return vaddr - bin->baddr;
		}
		return vaddr;
	}
	for (i = 0; i < bin->ehdr.e_phnum; ++i) {
		Elf_(Phdr) *p = &bin->phdr[i];
		if (!p) {
			break;
		}
		if (p->p_type == PT_LOAD && is_in_vphdr (p, vaddr)) {
			if (!p->p_offset && !p->p_vaddr) {
				continue;
			}
			return p->p_offset + vaddr - p->p_vaddr;
		}
	}
	return vaddr;
}
