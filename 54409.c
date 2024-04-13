get_offset_from_virtaddr(struct magic_set *ms, int swap, int clazz, int fd,
    off_t off, int num, off_t fsize, uint64_t virtaddr)
{
	Elf32_Phdr ph32;
	Elf64_Phdr ph64;

	/*
	 * Loop through all the program headers and find the header with
	 * virtual address in which the "virtaddr" belongs to.
	 */
	for ( ; num; num--) {
		if (pread(fd, xph_addr, xph_sizeof, off) < (ssize_t)xph_sizeof) {
			file_badread(ms);
			return -1;
		}
		off += xph_sizeof;

		if (fsize != SIZE_UNKNOWN && xph_offset > fsize) {
			/* Perhaps warn here */
			continue;
		}

		if (virtaddr >= xph_vaddr && virtaddr < xph_vaddr + xph_filesz)
			return xph_offset + (virtaddr - xph_vaddr);
	}
	return 0;
}
