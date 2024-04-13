ELFOBJ* Elf_(r_bin_elf_new_buf)(RBuffer *buf, bool verbose) {
	ELFOBJ *bin = R_NEW0 (ELFOBJ);
	bin->kv = sdb_new0 ();
	bin->b = r_buf_new ();
	bin->size = (ut32)buf->length;
	bin->verbose = verbose;
	if (!r_buf_set_bytes (bin->b, buf->buf, buf->length)) {
		return Elf_(r_bin_elf_free) (bin);
	}
	if (!elf_init (bin)) {
		return Elf_(r_bin_elf_free) (bin);
	}
	return bin;
}
