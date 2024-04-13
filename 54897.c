static RBinElfSymbol *Elf_(r_bin_elf_get_phdr_symbols)(ELFOBJ *bin) {
	if (!bin) {
		return NULL;
	}
	if (bin->phdr_symbols) {
		return bin->phdr_symbols;
	}
	bin->phdr_symbols = get_symbols_from_phdr (bin, R_BIN_ELF_SYMBOLS);
	return bin->phdr_symbols;
}
