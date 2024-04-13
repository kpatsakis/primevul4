static int is_in_pphdr (Elf_(Phdr) *p, ut64 addr) {
	return addr >= p->p_offset && addr < p->p_offset + p->p_memsz;
}
