static int init_shdr(ELFOBJ *bin) {
	ut32 shdr_size;
	ut8 shdr[sizeof (Elf_(Shdr))] = {0};
	int i, j, len;

	if (!bin || bin->shdr) {
		return true;
	}
	if (!UT32_MUL (&shdr_size, bin->ehdr.e_shnum, sizeof (Elf_(Shdr)))) {
		return false;
	}
	if (shdr_size < 1) {
		return false;
	}
	if (shdr_size > bin->size) {
		return false;
	}
	if (bin->ehdr.e_shoff > bin->size) {
		return false;
	}
	if (bin->ehdr.e_shoff + shdr_size > bin->size) {
		return false;
	}
	if (!(bin->shdr = calloc (1, shdr_size + 1))) {
		perror ("malloc (shdr)");
		return false;
	}
	sdb_num_set (bin->kv, "elf_shdr.offset", bin->ehdr.e_shoff, 0);
	sdb_num_set (bin->kv, "elf_shdr.size", sizeof (Elf_(Shdr)), 0);
	sdb_set (bin->kv, "elf_s_type.cparse", "enum elf_s_type {SHT_NULL=0,SHT_PROGBITS=1,"
			"SHT_SYMTAB=2,SHT_STRTAB=3,SHT_RELA=4,SHT_HASH=5,SHT_DYNAMIC=6,SHT_NOTE=7,"
			"SHT_NOBITS=8,SHT_REL=9,SHT_SHLIB=10,SHT_DYNSYM=11,SHT_LOOS=0x60000000,"
			"SHT_HIOS=0x6fffffff,SHT_LOPROC=0x70000000,SHT_HIPROC=0x7fffffff};", 0);

	for (i = 0; i < bin->ehdr.e_shnum; i++) {
		j = 0;
		len = r_buf_read_at (bin->b, bin->ehdr.e_shoff + i * sizeof (Elf_(Shdr)), shdr, sizeof (Elf_(Shdr)));
		if (len < 1) {
			bprintf ("Warning: read (shdr) at 0x%"PFMT64x"\n", (ut64) bin->ehdr.e_shoff);
			R_FREE (bin->shdr);
			return false;
		}
		bin->shdr[i].sh_name = READ32 (shdr, j)
		bin->shdr[i].sh_type = READ32 (shdr, j)
#if R_BIN_ELF64
		bin->shdr[i].sh_flags = READ64 (shdr, j)
		bin->shdr[i].sh_addr = READ64 (shdr, j)
		bin->shdr[i].sh_offset = READ64 (shdr, j)
		bin->shdr[i].sh_size = READ64 (shdr, j)
		bin->shdr[i].sh_link = READ32 (shdr, j)
		bin->shdr[i].sh_info = READ32 (shdr, j)
		bin->shdr[i].sh_addralign = READ64 (shdr, j)
		bin->shdr[i].sh_entsize = READ64 (shdr, j)
#else
		bin->shdr[i].sh_flags = READ32 (shdr, j)
		bin->shdr[i].sh_addr = READ32 (shdr, j)
		bin->shdr[i].sh_offset = READ32 (shdr, j)
		bin->shdr[i].sh_size = READ32 (shdr, j)
		bin->shdr[i].sh_link = READ32 (shdr, j)
		bin->shdr[i].sh_info = READ32 (shdr, j)
		bin->shdr[i].sh_addralign = READ32 (shdr, j)
		bin->shdr[i].sh_entsize = READ32 (shdr, j)
#endif
	}

#if R_BIN_ELF64
	sdb_set (bin->kv, "elf_s_flags_64.cparse", "enum elf_s_flags_64 {SF64_None=0,SF64_Exec=1,"
			"SF64_Alloc=2,SF64_Alloc_Exec=3,SF64_Write=4,SF64_Write_Exec=5,"
			"SF64_Write_Alloc=6,SF64_Write_Alloc_Exec=7};", 0);
	sdb_set (bin->kv, "elf_shdr.format", "x[4]E[8]Eqqqxxqq name (elf_s_type)type"
			" (elf_s_flags_64)flags addr offset size link info addralign entsize", 0);
#else
	sdb_set (bin->kv, "elf_s_flags_32.cparse", "enum elf_s_flags_32 {SF32_None=0,SF32_Exec=1,"
			"SF32_Alloc=2,SF32_Alloc_Exec=3,SF32_Write=4,SF32_Write_Exec=5,"
			"SF32_Write_Alloc=6,SF32_Write_Alloc_Exec=7};", 0);
	sdb_set (bin->kv, "elf_shdr.format", "x[4]E[4]Exxxxxxx name (elf_s_type)type"
			" (elf_s_flags_32)flags addr offset size link info addralign entsize", 0);
#endif
	return true;
}
