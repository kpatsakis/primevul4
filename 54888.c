ut64 Elf_(r_bin_elf_get_entry_offset)(ELFOBJ *bin) {
	ut64 entry;
	if (!bin) {
		return 0LL;
	}
	entry = bin->ehdr.e_entry;
	if (!entry) {
		entry = Elf_(r_bin_elf_get_section_offset)(bin, ".init.text");
		if (entry != UT64_MAX) {
			return entry;
		}
		entry = Elf_(r_bin_elf_get_section_offset)(bin, ".text");
		if (entry != UT64_MAX) {
			return entry;
		}
		entry = Elf_(r_bin_elf_get_section_offset)(bin, ".init");
		if (entry != UT64_MAX) {
			return entry;
		}
		if (entry == UT64_MAX) {
			return 0;
		}
	}
	return Elf_(r_bin_elf_v2p) (bin, entry);
}
