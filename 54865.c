static int is_in_vphdr (Elf_(Phdr) *p, ut64 addr) {
	return addr >= p->p_vaddr && addr < p->p_vaddr + p->p_memsz;
}
