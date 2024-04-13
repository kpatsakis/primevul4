ELFOBJ* Elf_(r_bin_elf_new)(const char* file, bool verbose) {
	ut8 *buf;
	int size;
	ELFOBJ *bin = R_NEW0 (ELFOBJ);
	if (!bin) {
		return NULL;
	}
	memset (bin, 0, sizeof (ELFOBJ));
	bin->file = file;
	if (!(buf = (ut8*)r_file_slurp (file, &size))) {
		return Elf_(r_bin_elf_free) (bin);
	}
	bin->size = size;
	bin->verbose = verbose;
	bin->b = r_buf_new ();
	if (!r_buf_set_bytes (bin->b, buf, bin->size)) {
		free (buf);
		return Elf_(r_bin_elf_free) (bin);
	}
	if (!elf_init (bin)) {
		free (buf);
		return Elf_(r_bin_elf_free) (bin);
	}
	free (buf);
	return bin;
}
