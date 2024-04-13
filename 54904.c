RBinElfSymbol *Elf_(r_bin_elf_get_symbols)(ELFOBJ *bin) {
	if (!bin->g_symbols) {
		bin->g_symbols = Elf_(_r_bin_elf_get_symbols_imports) (bin, R_BIN_ELF_SYMBOLS);
	}
	return bin->g_symbols;
}
