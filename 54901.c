ut64 Elf_(r_bin_elf_get_section_offset)(ELFOBJ *bin, const char *section_name) {
	RBinElfSection *section = get_section_by_name (bin, section_name);
	if (!section) return UT64_MAX;
	return section->offset;
}
