RBinElfSymbol *Elf_(r_bin_elf_get_imports)(ELFOBJ *bin) {
	if (!bin->g_imports) {
		bin->g_imports = Elf_(_r_bin_elf_get_symbols_imports) (bin, R_BIN_ELF_IMPORTS);
	}
	return bin->g_imports;
}
