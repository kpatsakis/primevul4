ut64 Elf_(r_bin_elf_get_init_offset)(ELFOBJ *bin) {
	ut64 entry = Elf_(r_bin_elf_get_entry_offset) (bin);
	ut8 buf[512];
	if (!bin) {
		return 0LL;
	}
	if (r_buf_read_at (bin->b, entry + 16, buf, sizeof (buf)) < 1) {
		bprintf ("Warning: read (init_offset)\n");
		return 0;
	}
	if (buf[0] == 0x68) { // push // x86 only
		ut64 addr;
		memmove (buf, buf+1, 4);
		addr = (ut64)r_read_le32 (buf);
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
	return 0;
}
