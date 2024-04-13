static ut64 getmainsymbol(ELFOBJ *bin) {
	struct r_bin_elf_symbol_t *symbol;
	int i;
	if (!(symbol = Elf_(r_bin_elf_get_symbols) (bin))) {
		return UT64_MAX;
	}
	for (i = 0; !symbol[i].last; i++) {
		if (!strcmp (symbol[i].name, "main")) {
			ut64 paddr = symbol[i].offset;
			return Elf_(r_bin_elf_p2v) (bin, paddr);
		}
	}
	return UT64_MAX;
}
