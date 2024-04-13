static RBinElfSection* get_sections_from_phdr(ELFOBJ *bin) {
	RBinElfSection *ret;
	int i, num_sections = 0;
	ut64 reldyn = 0, relava = 0, pltgotva = 0, relva = 0;
	ut64 reldynsz = 0, relasz = 0, pltgotsz = 0;
	if (!bin || !bin->phdr || !bin->ehdr.e_phnum)
		return NULL;

	for (i = 0; i < bin->dyn_entries; i++) {
		switch (bin->dyn_buf[i].d_tag) {
		case DT_REL:
			reldyn = bin->dyn_buf[i].d_un.d_ptr;
			num_sections++;
			break;
		case DT_RELA:
			relva = bin->dyn_buf[i].d_un.d_ptr;
			num_sections++;
			break;
		case DT_RELSZ:
			reldynsz = bin->dyn_buf[i].d_un.d_val;
			break;
		case DT_RELASZ:
			relasz = bin->dyn_buf[i].d_un.d_val;
			break;
		case DT_PLTGOT:
			pltgotva = bin->dyn_buf[i].d_un.d_ptr;
			num_sections++;
			break;
		case DT_PLTRELSZ:
			pltgotsz = bin->dyn_buf[i].d_un.d_val;
			break;
		case DT_JMPREL:
			relava = bin->dyn_buf[i].d_un.d_ptr;
			num_sections++;
			break;
		default: break;
		}
	}
	ret = calloc (num_sections + 1, sizeof(RBinElfSection));
	if (!ret) {
		return NULL;
	}
	i = 0;
	if (reldyn) {
		ret[i].offset = Elf_(r_bin_elf_v2p) (bin, reldyn);
		ret[i].rva = reldyn;
		ret[i].size = reldynsz;
		strcpy (ret[i].name, ".rel.dyn");
		ret[i].last = 0;
		i++;
	}
	if (relava) {
		ret[i].offset = Elf_(r_bin_elf_v2p) (bin, relava);
		ret[i].rva = relava;
		ret[i].size = pltgotsz;
		strcpy (ret[i].name, ".rela.plt");
		ret[i].last = 0;
		i++;
	}
	if (relva) {
		ret[i].offset = Elf_(r_bin_elf_v2p) (bin, relva);
		ret[i].rva = relva;
		ret[i].size = relasz;
		strcpy (ret[i].name, ".rel.plt");
		ret[i].last = 0;
		i++;
	}
	if (pltgotva) {
		ret[i].offset = Elf_(r_bin_elf_v2p) (bin, pltgotva);
		ret[i].rva = pltgotva;
		ret[i].size = pltgotsz;
		strcpy (ret[i].name, ".got.plt");
		ret[i].last = 0;
		i++;
	}
	ret[i].last = 1;

	return ret;
}
