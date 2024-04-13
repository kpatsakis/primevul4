static int init_dynamic_section(struct Elf_(r_bin_elf_obj_t) *bin) {
	Elf_(Dyn) *dyn = NULL;
	Elf_(Dyn) d = {0};
	Elf_(Addr) strtabaddr = 0;
	ut64 offset = 0;
	char *strtab = NULL;
	size_t relentry = 0, strsize = 0;
	int entries;
	int i, j, len, r;
	ut8 sdyn[sizeof (Elf_(Dyn))] = {0};
	ut32 dyn_size = 0;

	if (!bin || !bin->phdr || !bin->ehdr.e_phnum) {
		return false;
	}
	for (i = 0; i < bin->ehdr.e_phnum ; i++) {
		if (bin->phdr[i].p_type == PT_DYNAMIC) {
			dyn_size = bin->phdr[i].p_filesz;
			break;
		}
	}
	if (i == bin->ehdr.e_phnum) {
		return false;
	}
	if (bin->phdr[i].p_filesz > bin->size) {
		return false;
	}
	if (bin->phdr[i].p_offset > bin->size) {
		return false;
	}
	if (bin->phdr[i].p_offset + sizeof(Elf_(Dyn)) > bin->size) {
		return false;
	}
	for (entries = 0; entries < (dyn_size / sizeof (Elf_(Dyn))); entries++) {
		j = 0;
		len = r_buf_read_at (bin->b, bin->phdr[i].p_offset + entries * sizeof (Elf_(Dyn)), sdyn, sizeof (Elf_(Dyn)));
		if (len < 1) {
			goto beach;
		}
#if R_BIN_ELF64
		d.d_tag = READ64 (sdyn, j)
#else
		d.d_tag = READ32 (sdyn, j)
#endif
		if (d.d_tag == DT_NULL) {
			break;
		}
	}
	if (entries < 1) {
		return false;
	}
	dyn = (Elf_(Dyn)*)calloc (entries, sizeof (Elf_(Dyn)));
	if (!dyn) {
		return false;
	}
	if (!UT32_MUL (&dyn_size, entries, sizeof (Elf_(Dyn)))) {
		goto beach;
	}
	if (!dyn_size) {
		goto beach;
	}
	offset = Elf_(r_bin_elf_v2p) (bin, bin->phdr[i].p_vaddr);
	if (offset > bin->size || offset + dyn_size > bin->size) {
		goto beach;
	}
	for (i = 0; i < entries; i++) {
		j = 0;
		r_buf_read_at (bin->b, offset + i * sizeof (Elf_(Dyn)), sdyn, sizeof (Elf_(Dyn)));
		if (len < 1) {
			bprintf("Warning: read (dyn)\n");
		}
#if R_BIN_ELF64
		dyn[i].d_tag = READ64 (sdyn, j)
		dyn[i].d_un.d_ptr = READ64 (sdyn, j)
#else
		dyn[i].d_tag = READ32 (sdyn, j)
		dyn[i].d_un.d_ptr = READ32 (sdyn, j)
#endif

		switch (dyn[i].d_tag) {
		case DT_STRTAB: strtabaddr = Elf_(r_bin_elf_v2p) (bin, dyn[i].d_un.d_ptr); break;
		case DT_STRSZ: strsize = dyn[i].d_un.d_val; break;
		case DT_PLTREL: bin->is_rela = dyn[i].d_un.d_val; break;
		case DT_RELAENT: relentry = dyn[i].d_un.d_val; break;
		default:
			if ((dyn[i].d_tag >= DT_VERSYM) && (dyn[i].d_tag <= DT_VERNEEDNUM)) {
				bin->version_info[DT_VERSIONTAGIDX (dyn[i].d_tag)] = dyn[i].d_un.d_val;
			}
			break;
		}
	}
	if (!bin->is_rela) {
		bin->is_rela = sizeof (Elf_(Rela)) == relentry? DT_RELA : DT_REL;
	}
	if (!strtabaddr || strtabaddr > bin->size || strsize > ST32_MAX || !strsize || strsize > bin->size) {
		if (!strtabaddr) {
			bprintf ("Warning: section.shstrtab not found or invalid\n");
		}
		goto beach;
	}
	strtab = (char *)calloc (1, strsize + 1);
	if (!strtab) {
		goto beach;
	}
	if (strtabaddr + strsize > bin->size) {
		free (strtab);
		goto beach;
	}
	r = r_buf_read_at (bin->b, strtabaddr, (ut8 *)strtab, strsize);
	if (r < 1) {
		free (strtab);
		goto beach;
	}
	bin->dyn_buf = dyn;
	bin->dyn_entries = entries;
	bin->strtab = strtab;
	bin->strtab_size = strsize;
	r = Elf_(r_bin_elf_has_relro)(bin);
	switch (r) {
	case R_ELF_FULL_RELRO:
		sdb_set (bin->kv, "elf.relro", "full", 0);
		break;
	case R_ELF_PART_RELRO:
		sdb_set (bin->kv, "elf.relro", "partial", 0);
		break;
	default:
		sdb_set (bin->kv, "elf.relro", "no", 0);
		break;
	}
	sdb_num_set (bin->kv, "elf_strtab.offset", strtabaddr, 0);
	sdb_num_set (bin->kv, "elf_strtab.size", strsize, 0);
	return true;
beach:
	free (dyn);
	return false;
}
