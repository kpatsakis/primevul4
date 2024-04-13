int Elf_(r_bin_elf_is_big_endian)(ELFOBJ *bin) {
	return (bin->ehdr.e_ident[EI_DATA] == ELFDATA2MSB);
}
