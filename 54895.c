ut64 Elf_(r_bin_elf_get_main_offset)(ELFOBJ *bin) {
	ut64 entry = Elf_(r_bin_elf_get_entry_offset) (bin);
	ut8 buf[512];
	if (!bin) {
		return 0LL;
	}
	if (entry > bin->size || (entry + sizeof (buf)) > bin->size) {
		return 0;
	}
	if (r_buf_read_at (bin->b, entry, buf, sizeof (buf)) < 1) {
		bprintf ("Warning: read (main)\n");
		return 0;
	}
	if (buf[0x18+3] == 0x58 && buf[0x2f] == 0x00) {
		ut32 entry_vaddr = Elf_(r_bin_elf_p2v) (bin, entry);
		ut32 main_addr = r_read_le32 (&buf[0x30]);
		if ((main_addr >> 16) == (entry_vaddr >> 16)) {
			return Elf_(r_bin_elf_v2p) (bin, main_addr);
		}
	}

	ut64 text = Elf_(r_bin_elf_get_section_offset)(bin, ".text");
	ut64 text_end = text + bin->size;

	if (entry & 1 && !memcmp (buf, "\xf0\x00\x0b\x4f\xf0\x00", 6)) {
		ut32 * ptr = (ut32*)(buf+40-1);
		if (*ptr &1) {
			return Elf_(r_bin_elf_v2p) (bin, *ptr -1);
		}
	}
	if (!memcmp (buf, "\x00\xb0\xa0\xe3\x00\xe0\xa0\xe3", 8)) {
		ut32 *addr = (ut32*)(buf+0x34);
		/*
		   0x00012000    00b0a0e3     mov fp, 0
		   0x00012004    00e0a0e3     mov lr, 0
		*/
		if (*addr > text && *addr < (text_end)) {
			return Elf_(r_bin_elf_v2p) (bin, *addr);
		}
	}

	/* get .got, calculate offset of main symbol */
	if (!memcmp (buf, "\x21\x00\xe0\x03\x01\x00\x11\x04", 8)) {

		/*
		    assuming the startup code looks like
		        got = gp-0x7ff0
		        got[index__libc_start_main] ( got[index_main] );

		    looking for the instruction generating the first argument to find main
		        lw a0, offset(gp)
		*/

		ut64 got_offset;
		if ((got_offset = Elf_(r_bin_elf_get_section_offset) (bin, ".got")) != -1 ||
		    (got_offset = Elf_(r_bin_elf_get_section_offset) (bin, ".got.plt")) != -1)
		{
			const ut64 gp = got_offset + 0x7ff0;
			unsigned i;
			for (i = 0; i < sizeof(buf) / sizeof(buf[0]); i += 4) {
				const ut32 instr = r_read_le32 (&buf[i]);
				if ((instr & 0xffff0000) == 0x8f840000) { // lw a0, offset(gp)
					const short delta = instr & 0x0000ffff;
					r_buf_read_at (bin->b, /* got_entry_offset = */ gp + delta, buf, 4);
					return Elf_(r_bin_elf_v2p) (bin, r_read_le32 (&buf[0]));
				}
			}
		}

		return 0;
	}
	if (!memcmp (buf, "\x24\xc0\x9f\xe5\x00\xb0\xa0\xe3", 8)) {
		ut64 addr = r_read_le32 (&buf[48]);
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
	if (buf[0] == 0xe8 && !memcmp (buf + 5, "\x50\xe8\x00\x00\x00\x00\xb8\x01\x00\x00\x00\x53", 12)) {
		size_t SIZEOF_CALL = 5;
		ut64 rel_addr = (ut64)((int)(buf[1] + (buf[2] << 8) + (buf[3] << 16) + (buf[4] << 24)));
		ut64 addr = Elf_(r_bin_elf_p2v)(bin, entry + SIZEOF_CALL);
		addr += rel_addr;
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
	if (buf[0x00] == 0x48 && buf[0x1e] == 0x8d && buf[0x11] == 0xe8) {
		ut32 *pmain = (ut32*)(buf + 0x30);
		ut64 vmain = Elf_(r_bin_elf_p2v) (bin, (ut64)*pmain);
		ut64 ventry = Elf_(r_bin_elf_p2v) (bin, entry);
		if (vmain >> 16 == ventry >> 16) {
			return (ut64)vmain;
		}
	}
	if (buf[0x1d] == 0x48 && buf[0x1e] == 0x8b) {
		if (!memcmp (buf, "\x31\xed\x49\x89", 4)) {// linux
			ut64 maddr, baddr;
			ut8 n32s[sizeof (ut32)] = {0};
			maddr = entry + 0x24 + r_read_le32 (buf + 0x20);
			if (r_buf_read_at (bin->b, maddr, n32s, sizeof (ut32)) == -1) {
				bprintf ("Warning: read (maddr) 2\n");
				return 0;
			}
			maddr = (ut64)r_read_le32 (&n32s[0]);
			baddr = (bin->ehdr.e_entry >> 16) << 16;
			if (bin->phdr) {
				baddr = Elf_(r_bin_elf_get_baddr) (bin);
			}
			maddr += baddr;
			return maddr;
		}
	}
#if R_BIN_ELF64
	if (!memcmp (buf, "\x49\x89\xd9", 3) && buf[156] == 0xe8) { // openbsd
		return r_read_le32 (&buf[157]) + entry + 156 + 5;
	}
	if (!memcmp (buf+29, "\x48\xc7\xc7", 3)) { // linux
		ut64 addr = (ut64)r_read_le32 (&buf[29 + 3]);
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
#else
	if (buf[23] == '\x68') {
		ut64 addr = (ut64)r_read_le32 (&buf[23 + 1]);
		return Elf_(r_bin_elf_v2p) (bin, addr);
	}
#endif
	/* linux64 pie main -- probably buggy in some cases */
	if (buf[29] == 0x48 && buf[30] == 0x8d) { // lea rdi, qword [rip-0x21c4]
		ut8 *p = buf + 32;
		st32 maindelta = (st32)r_read_le32 (p);
		ut64 vmain = (ut64)(entry + 29 + maindelta) + 7;
		ut64 ventry = Elf_(r_bin_elf_p2v) (bin, entry);
		if (vmain>>16 == ventry>>16) {
			return (ut64)vmain;
		}
	}
	/* find sym.main if possible */
	{
		ut64 m = getmainsymbol (bin);
		if (m != UT64_MAX) return m;
	}
	return UT64_MAX;
}
