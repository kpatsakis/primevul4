char* Elf_(r_bin_elf_get_file_type)(ELFOBJ *bin) {
	ut32 e_type;
	if (!bin) {
		return NULL;
	}
	e_type = (ut32)bin->ehdr.e_type; // cast to avoid warn in iphone-gcc, must be ut16
	switch (e_type) {
	case ET_NONE: return strdup ("NONE (None)");
	case ET_REL:  return strdup ("REL (Relocatable file)");
	case ET_EXEC: return strdup ("EXEC (Executable file)");
	case ET_DYN:  return strdup ("DYN (Shared object file)");
	case ET_CORE: return strdup ("CORE (Core file)");
	}
	if ((e_type >= ET_LOPROC) && (e_type <= ET_HIPROC)) {
		return r_str_newf ("Processor Specific: %x", e_type);
	}
	if ((e_type >= ET_LOOS) && (e_type <= ET_HIOS)) {
		return r_str_newf ("OS Specific: %x", e_type);
	}
	return r_str_newf ("<unknown>: %x", e_type);
}
