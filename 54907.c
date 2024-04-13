int Elf_(r_bin_elf_has_relro)(ELFOBJ *bin) {
	int i;
	bool haveBindNow = false;
	bool haveGnuRelro = false;
	if (bin && bin->dyn_buf) {
		for (i = 0; i < bin->dyn_entries; i++) {
			switch (bin->dyn_buf[i].d_tag) {
			case DT_BIND_NOW:
				haveBindNow = true;
				break;
			case DT_FLAGS:
				for (i++; i < bin->dyn_entries ; i++) {
					ut32 dTag = bin->dyn_buf[i].d_tag;
					if (!dTag) {
						break;
					}
					switch (dTag) {
					case DT_FLAGS_1:
						if (bin->dyn_buf[i].d_un.d_val & DF_1_NOW) {
							haveBindNow = true;
							break;
						}
					}
				}
				break;
			}
		}
	}
	if (bin && bin->phdr) {
		for (i = 0; i < bin->ehdr.e_phnum; i++) {
			if (bin->phdr[i].p_type == PT_GNU_RELRO) {
				haveGnuRelro = true;
				break;
			}
		}
	}
	if (haveGnuRelro) {
		if (haveBindNow) {
			return R_ELF_FULL_RELRO;
		}
		return R_ELF_PART_RELRO;
	}
	return R_ELF_NO_RELRO;
}
