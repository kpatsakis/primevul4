static int handle_e_ident(ELFOBJ *bin) {
	return !strncmp ((char *)bin->ehdr.e_ident, ELFMAG, SELFMAG) ||
		   !strncmp ((char *)bin->ehdr.e_ident, CGCMAG, SCGCMAG);
}
