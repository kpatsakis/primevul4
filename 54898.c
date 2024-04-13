char *Elf_(r_bin_elf_get_rpath)(ELFOBJ *bin) {
	char *ret = NULL;
	int j;

	if (!bin || !bin->phdr || !bin->dyn_buf || !bin->strtab) {
		return NULL;
	}
	for (j = 0; j< bin->dyn_entries; j++) {
		if (bin->dyn_buf[j].d_tag == DT_RPATH || bin->dyn_buf[j].d_tag == DT_RUNPATH) {
			if (!(ret = calloc (1, ELF_STRING_LENGTH))) {
				perror ("malloc (rpath)");
				return NULL;
			}
			if (bin->dyn_buf[j].d_un.d_val > bin->strtab_size) {
				free (ret);
				return NULL;
			}
			strncpy (ret, bin->strtab + bin->dyn_buf[j].d_un.d_val, ELF_STRING_LENGTH);
			ret[ELF_STRING_LENGTH - 1] = '\0';
			break;
		}
	}
	return ret;
}
