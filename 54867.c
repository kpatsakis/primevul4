static int elf_init(ELFOBJ *bin) {
	bin->phdr = NULL;
	bin->shdr = NULL;
	bin->strtab = NULL;
	bin->shstrtab = NULL;
	bin->strtab_size = 0;
	bin->strtab_section = NULL;
	bin->dyn_buf = NULL;
	bin->dynstr = NULL;
	ZERO_FILL (bin->version_info);

	bin->g_sections = NULL;
	bin->g_symbols = NULL;
	bin->g_imports = NULL;
	/* bin is not an ELF */
	if (!init_ehdr (bin)) {
		return false;
	}
	if (!init_phdr (bin)) {
		bprintf ("Warning: Cannot initialize program headers\n");
	}
	if (!init_shdr (bin)) {
		bprintf ("Warning: Cannot initialize section headers\n");
	}
	if (!init_strtab (bin)) {
		bprintf ("Warning: Cannot initialize strings table\n");
	}
	if (!init_dynstr (bin)) {
		bprintf ("Warning: Cannot initialize dynamic strings\n");
	}
	bin->baddr = Elf_(r_bin_elf_get_baddr) (bin);
	if (!init_dynamic_section (bin) && !Elf_(r_bin_elf_get_static)(bin))
		bprintf ("Warning: Cannot initialize dynamic section\n");

	bin->imports_by_ord_size = 0;
	bin->imports_by_ord = NULL;
	bin->symbols_by_ord_size = 0;
	bin->symbols_by_ord = NULL;
	bin->g_sections = Elf_(r_bin_elf_get_sections) (bin);
	bin->boffset = Elf_(r_bin_elf_get_boffset) (bin);
	sdb_ns_set (bin->kv, "versioninfo", store_versioninfo (bin));

	return true;
}
