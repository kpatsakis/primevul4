static int init_phdr(ELFOBJ *bin) {
	ut32 phdr_size;
	ut8 phdr[sizeof (Elf_(Phdr))] = {0};
	int i, j, len;
	if (!bin->ehdr.e_phnum) {
		return false;
	}
	if (bin->phdr) {
		return true;
	}
	if (!UT32_MUL (&phdr_size, (ut32)bin->ehdr.e_phnum, sizeof (Elf_(Phdr)))) {
		return false;
	}
	if (!phdr_size) {
		return false;
	}
	if (phdr_size > bin->size) {
		return false;
	}
	if (phdr_size > (ut32)bin->size) {
		return false;
	}
	if (bin->ehdr.e_phoff > bin->size) {
		return false;
	}
	if (bin->ehdr.e_phoff + phdr_size > bin->size) {
		return false;
	}
	if (!(bin->phdr = calloc (phdr_size, 1))) {
		perror ("malloc (phdr)");
		return false;
	}
	for (i = 0; i < bin->ehdr.e_phnum; i++) {
		j = 0;
		len = r_buf_read_at (bin->b, bin->ehdr.e_phoff + i * sizeof (Elf_(Phdr)), phdr, sizeof (Elf_(Phdr)));
		if (len < 1) {
			bprintf ("Warning: read (phdr)\n");
			R_FREE (bin->phdr);
			return false;
		}
		bin->phdr[i].p_type = READ32 (phdr, j)
#if R_BIN_ELF64
		bin->phdr[i].p_flags = READ32 (phdr, j)
		bin->phdr[i].p_offset = READ64 (phdr, j)
		bin->phdr[i].p_vaddr = READ64 (phdr, j)
		bin->phdr[i].p_paddr = READ64 (phdr, j)
		bin->phdr[i].p_filesz = READ64 (phdr, j)
		bin->phdr[i].p_memsz = READ64 (phdr, j)
		bin->phdr[i].p_align = READ64 (phdr, j)
#else
		bin->phdr[i].p_offset = READ32 (phdr, j)
		bin->phdr[i].p_vaddr = READ32 (phdr, j)
		bin->phdr[i].p_paddr = READ32 (phdr, j)
		bin->phdr[i].p_filesz = READ32 (phdr, j)
		bin->phdr[i].p_memsz = READ32 (phdr, j)
		bin->phdr[i].p_flags = READ32 (phdr, j)
		bin->phdr[i].p_align = READ32 (phdr, j)
#endif
	}
	sdb_num_set (bin->kv, "elf_phdr.offset", bin->ehdr.e_phoff, 0);
	sdb_num_set (bin->kv, "elf_phdr.size", sizeof (Elf_(Phdr)), 0);
	sdb_set (bin->kv, "elf_p_type.cparse", "enum elf_p_type {PT_NULL=0,PT_LOAD=1,PT_DYNAMIC=2,"
			"PT_INTERP=3,PT_NOTE=4,PT_SHLIB=5,PT_PHDR=6,PT_LOOS=0x60000000,"
			"PT_HIOS=0x6fffffff,PT_LOPROC=0x70000000,PT_HIPROC=0x7fffffff};", 0);
	sdb_set (bin->kv, "elf_p_flags.cparse", "enum elf_p_flags {PF_None=0,PF_Exec=1,"
			"PF_Write=2,PF_Write_Exec=3,PF_Read=4,PF_Read_Exec=5,PF_Read_Write=6,"
			"PF_Read_Write_Exec=7};", 0);
#if R_BIN_ELF64
	sdb_set (bin->kv, "elf_phdr.format", "[4]E[4]Eqqqqqq (elf_p_type)type (elf_p_flags)flags"
			" offset vaddr paddr filesz memsz align", 0);
#else
	sdb_set (bin->kv, "elf_phdr.format", "[4]Exxxxx[4]Ex (elf_p_type)type offset vaddr paddr"
			" filesz memsz (elf_p_flags)flags align", 0);
#endif
	return true;
}
