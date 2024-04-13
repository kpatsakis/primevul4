static void fill_symbol_bind_and_type (struct r_bin_elf_symbol_t *ret, Elf_(Sym) *sym) {
	#define s_bind(x) ret->bind = x
	#define s_type(x) ret->type = x
	switch (ELF_ST_BIND(sym->st_info)) {
	case STB_LOCAL:  s_bind ("LOCAL"); break;
	case STB_GLOBAL: s_bind ("GLOBAL"); break;
	case STB_WEAK:   s_bind ("WEAK"); break;
	case STB_NUM:    s_bind ("NUM"); break;
	case STB_LOOS:   s_bind ("LOOS"); break;
	case STB_HIOS:   s_bind ("HIOS"); break;
	case STB_LOPROC: s_bind ("LOPROC"); break;
	case STB_HIPROC: s_bind ("HIPROC"); break;
	default:         s_bind ("UNKNOWN");
	}
	switch (ELF_ST_TYPE (sym->st_info)) {
	case STT_NOTYPE:  s_type ("NOTYPE"); break;
	case STT_OBJECT:  s_type ("OBJECT"); break;
	case STT_FUNC:    s_type ("FUNC"); break;
	case STT_SECTION: s_type ("SECTION"); break;
	case STT_FILE:    s_type ("FILE"); break;
	case STT_COMMON:  s_type ("COMMON"); break;
	case STT_TLS:     s_type ("TLS"); break;
	case STT_NUM:     s_type ("NUM"); break;
	case STT_LOOS:    s_type ("LOOS"); break;
	case STT_HIOS:    s_type ("HIOS"); break;
	case STT_LOPROC:  s_type ("LOPROC"); break;
	case STT_HIPROC:  s_type ("HIPROC"); break;
	default:          s_type ("UNKNOWN");
	}
}
