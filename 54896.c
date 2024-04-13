char* Elf_(r_bin_elf_get_osabi_name)(ELFOBJ *bin) {
	switch (bin->ehdr.e_ident[EI_OSABI]) {
	case ELFOSABI_LINUX: return strdup("linux");
	case ELFOSABI_SOLARIS: return strdup("solaris");
	case ELFOSABI_FREEBSD: return strdup("freebsd");
	case ELFOSABI_HPUX: return strdup("hpux");
	}
	/* Hack to identify OS */
	if (needle (bin, "openbsd")) return strdup ("openbsd");
	if (needle (bin, "netbsd")) return strdup ("netbsd");
	if (needle (bin, "freebsd")) return strdup ("freebsd");
	if (noodle (bin, "BEOS:APP_VERSION")) return strdup ("beos");
	if (needle (bin, "GNU")) return strdup ("linux");
	return strdup ("linux");
}
