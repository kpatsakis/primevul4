RBinElfSection* Elf_(r_bin_elf_get_sections)(ELFOBJ *bin) {
	RBinElfSection *ret = NULL;
	char unknown_s[20], invalid_s[20];
	int i, nidx, unknown_c=0, invalid_c=0;
	if (!bin) {
		return NULL;
	}
	if (bin->g_sections) {
		return bin->g_sections;
	}
	if (!bin->shdr) {
		return get_sections_from_phdr (bin);
	}
	if (!(ret = calloc ((bin->ehdr.e_shnum + 1), sizeof (RBinElfSection)))) {
		return NULL;
	}
	for (i = 0; i < bin->ehdr.e_shnum; i++) {
		ret[i].offset = bin->shdr[i].sh_offset;
		ret[i].size = bin->shdr[i].sh_size;
		ret[i].align = bin->shdr[i].sh_addralign;
		ret[i].flags = bin->shdr[i].sh_flags;
		ret[i].link = bin->shdr[i].sh_link;
		ret[i].info = bin->shdr[i].sh_info;
		ret[i].type = bin->shdr[i].sh_type;
		if (bin->ehdr.e_type == ET_REL)	{
			ret[i].rva = bin->baddr + bin->shdr[i].sh_offset;
		} else {
			ret[i].rva = bin->shdr[i].sh_addr;
		}
		nidx = bin->shdr[i].sh_name;
#define SHNAME (int)bin->shdr[i].sh_name
#define SHNLEN ELF_STRING_LENGTH - 4
#define SHSIZE (int)bin->shstrtab_size
		if (nidx < 0 || !bin->shstrtab_section || !bin->shstrtab_size || nidx > bin->shstrtab_size) {
			snprintf (invalid_s, sizeof (invalid_s) - 4, "invalid%d", invalid_c);
			strncpy (ret[i].name, invalid_s, SHNLEN);
			invalid_c++;
		} else {
			if (bin->shstrtab && (SHNAME > 0) && (SHNAME < SHSIZE)) {
				strncpy (ret[i].name, &bin->shstrtab[SHNAME], SHNLEN);
			} else {
				if (bin->shdr[i].sh_type == SHT_NULL) {
					strncpy (ret[i].name, "", sizeof (ret[i].name) - 4);
				} else {
					snprintf (unknown_s, sizeof (unknown_s)-4, "unknown%d", unknown_c);
					strncpy (ret[i].name, unknown_s, sizeof (ret[i].name)-4);
					unknown_c++;
				}
			}
		}
		ret[i].name[ELF_STRING_LENGTH-2] = '\0';
		ret[i].last = 0;
	}
	ret[i].last = 1;
	return ret;
}
