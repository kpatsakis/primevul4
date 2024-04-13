static RBinElfSection* get_section_by_name(ELFOBJ *bin, const char *section_name) {
	int i;
	if (!bin->g_sections) {
		return NULL;
	}
	for (i = 0; !bin->g_sections[i].last; i++) {
		if (!strncmp (bin->g_sections[i].name, section_name, ELF_STRING_LENGTH-1)) {
			return &bin->g_sections[i];
		}
	}
	return NULL;
}
