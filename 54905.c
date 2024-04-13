ut8 *Elf_(r_bin_elf_grab_regstate)(ELFOBJ *bin, int *len) {
	if (bin->phdr) {
		int i;
		int num = bin->ehdr.e_phnum;
		for (i = 0; i < num; i++) {
			if (bin->phdr[i].p_type != PT_NOTE) {
				continue;
			}
			int bits = Elf_(r_bin_elf_get_bits)(bin);
			int regdelta = (bits == 64)? 0x84: 0x40; // x64 vs x32
			int regsize = 160; // for x86-64
			ut8 *buf = malloc (regsize);
			if (r_buf_read_at (bin->b, bin->phdr[i].p_offset + regdelta, buf, regsize) != regsize) {
				free (buf);
				bprintf ("Cannot read register state from CORE file\n");
				return NULL;
			}
			if (len) {
				*len = regsize;
			}
			return buf;
		}
	}
	bprintf ("Cannot find NOTE section\n");
	return NULL;
}
