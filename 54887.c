char* Elf_(r_bin_elf_get_data_encoding)(ELFOBJ *bin) {
	switch (bin->ehdr.e_ident[EI_DATA]) {
	case ELFDATANONE: return strdup ("none");
	case ELFDATA2LSB: return strdup ("2's complement, little endian");
	case ELFDATA2MSB: return strdup ("2's complement, big endian");
	default: return r_str_newf ("<unknown: %x>", bin->ehdr.e_ident[EI_DATA]);
	}
}
