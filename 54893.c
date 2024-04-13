RBinElfLib* Elf_(r_bin_elf_get_libs)(ELFOBJ *bin) {
	RBinElfLib *ret = NULL;
	int j, k;

	if (!bin || !bin->phdr || !bin->dyn_buf || !bin->strtab || *(bin->strtab+1) == '0') {
		return NULL;
	}
	for (j = 0, k = 0; j < bin->dyn_entries; j++)
		if (bin->dyn_buf[j].d_tag == DT_NEEDED) {
			RBinElfLib *r = realloc (ret, (k + 1) * sizeof (RBinElfLib));
			if (!r) {
				perror ("realloc (libs)");
				free (ret);
				return NULL;
			}
			ret = r;
			if (bin->dyn_buf[j].d_un.d_val > bin->strtab_size) {
				free (ret);
				return NULL;
			}
			strncpy (ret[k].name, bin->strtab + bin->dyn_buf[j].d_un.d_val, ELF_STRING_LENGTH);
			ret[k].name[ELF_STRING_LENGTH - 1] = '\0';
   			ret[k].last = 0;
			if (ret[k].name[0]) {
				k++;
			}
		}
	RBinElfLib *r = realloc (ret, (k + 1) * sizeof (RBinElfLib));
	if (!r) {
		perror ("realloc (libs)");
		free (ret);
		return NULL;
	}
	ret = r;
	ret[k].last = 1;
	return ret;
}
