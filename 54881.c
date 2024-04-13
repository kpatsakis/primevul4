static int init_strtab(ELFOBJ *bin) {
	if (bin->strtab || !bin->shdr) {
		return false;
	}
	if (bin->ehdr.e_shstrndx != SHN_UNDEF &&
		(bin->ehdr.e_shstrndx >= bin->ehdr.e_shnum ||
		(bin->ehdr.e_shstrndx >= SHN_LORESERVE &&
		bin->ehdr.e_shstrndx < SHN_HIRESERVE)))
		return false;

	/* sh_size must be lower than UT32_MAX and not equal to zero, to avoid bugs on malloc() */
	if (bin->shdr[bin->ehdr.e_shstrndx].sh_size > UT32_MAX) {
		return false;
	}
	if (!bin->shdr[bin->ehdr.e_shstrndx].sh_size) {
		return false;
	}
	bin->shstrtab_section = bin->strtab_section = &bin->shdr[bin->ehdr.e_shstrndx];
	bin->shstrtab_size = bin->strtab_section->sh_size;
	if (bin->shstrtab_size > bin->size) {
		return false;
	}
	if (!(bin->shstrtab = calloc (1, bin->shstrtab_size + 1))) {
		perror ("malloc");
		bin->shstrtab = NULL;
		return false;
	}
	if (bin->shstrtab_section->sh_offset > bin->size) {
		R_FREE (bin->shstrtab);
		return false;
	}

	if (bin->shstrtab_section->sh_offset +
		bin->shstrtab_section->sh_size  > bin->size) {
		R_FREE (bin->shstrtab);
		return false;
	}
	if (r_buf_read_at (bin->b, bin->shstrtab_section->sh_offset, (ut8*)bin->shstrtab,
				bin->shstrtab_section->sh_size + 1) < 1) {
		bprintf ("Warning: read (shstrtab) at 0x%"PFMT64x"\n",
				(ut64) bin->shstrtab_section->sh_offset);
		R_FREE (bin->shstrtab);
		return false;
	}
	bin->shstrtab[bin->shstrtab_section->sh_size] = '\0';

	sdb_num_set (bin->kv, "elf_shstrtab.offset", bin->shstrtab_section->sh_offset, 0);
	sdb_num_set (bin->kv, "elf_shstrtab.size", bin->shstrtab_section->sh_size, 0);

	return true;
}
