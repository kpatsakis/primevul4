static RBinElfSymbol* Elf_(_r_bin_elf_get_symbols_imports)(ELFOBJ *bin, int type) {
	ut32 shdr_size;
	int tsize, nsym, ret_ctr = 0, i, j, r, k, newsize;
	ut64 toffset;
	ut32 size = 0;
	RBinElfSymbol  *ret = NULL;
	Elf_(Shdr) *strtab_section = NULL;
	Elf_(Sym) *sym = NULL;
	ut8 s[sizeof (Elf_(Sym))] = { 0 };
	char *strtab = NULL;

	if (!bin || !bin->shdr || !bin->ehdr.e_shnum || bin->ehdr.e_shnum == 0xffff) {
		return (type == R_BIN_ELF_SYMBOLS)
				? Elf_(r_bin_elf_get_phdr_symbols) (bin)
				: Elf_(r_bin_elf_get_phdr_imports) (bin);
	}
	if (!UT32_MUL (&shdr_size, bin->ehdr.e_shnum, sizeof (Elf_(Shdr)))) {
		return false;
	}
	if (shdr_size + 8 > bin->size) {
		return false;
	}
	for (i = 0; i < bin->ehdr.e_shnum; i++) {
		if ((type == R_BIN_ELF_IMPORTS && bin->shdr[i].sh_type == (bin->ehdr.e_type == ET_REL ? SHT_SYMTAB : SHT_DYNSYM)) ||
				(type == R_BIN_ELF_SYMBOLS && bin->shdr[i].sh_type == (Elf_(r_bin_elf_get_stripped) (bin) ? SHT_DYNSYM : SHT_SYMTAB))) {
			if (bin->shdr[i].sh_link < 1) {
				/* oops. fix out of range pointers */
				continue;
			}
			if ((bin->shdr[i].sh_link * sizeof(Elf_(Shdr))) >= shdr_size) {
				/* oops. fix out of range pointers */
				continue;
			}
			strtab_section = &bin->shdr[bin->shdr[i].sh_link];
			if (strtab_section->sh_size > ST32_MAX || strtab_section->sh_size+8 > bin->size) {
				bprintf ("size (syms strtab)");
				free (ret);
				free (strtab);
				return NULL;
			}
			if (!strtab) {
				if (!(strtab = (char *)calloc (1, 8 + strtab_section->sh_size))) {
					bprintf ("malloc (syms strtab)");
					goto beach;
				}
				if (strtab_section->sh_offset > bin->size ||
						strtab_section->sh_offset + strtab_section->sh_size > bin->size) {
					goto beach;
				}
				if (r_buf_read_at (bin->b, strtab_section->sh_offset,
							(ut8*)strtab, strtab_section->sh_size) == -1) {
					bprintf ("Warning: read (syms strtab)\n");
					goto beach;
				}
			}

			newsize = 1 + bin->shdr[i].sh_size;
			if (newsize < 0 || newsize > bin->size) {
				bprintf ("invalid shdr %d size\n", i);
				goto beach;
			}
			nsym = (int)(bin->shdr[i].sh_size / sizeof (Elf_(Sym)));
			if (nsym < 0) {
				goto beach;
			}
			if (!(sym = (Elf_(Sym) *)calloc (nsym, sizeof (Elf_(Sym))))) {
				bprintf ("calloc (syms)");
				goto beach;
			}
			if (!UT32_MUL (&size, nsym, sizeof (Elf_(Sym)))) {
				goto beach;
			}
			if (size < 1 || size > bin->size) {
				goto beach;
			}
			if (bin->shdr[i].sh_offset > bin->size) {
				goto beach;
			}
			if (bin->shdr[i].sh_offset + size > bin->size) {
				goto beach;
			}
			for (j = 0; j < nsym; j++) {
				int k = 0;
				r = r_buf_read_at (bin->b, bin->shdr[i].sh_offset + j * sizeof (Elf_(Sym)), s, sizeof (Elf_(Sym)));
				if (r < 1) {
					bprintf ("Warning: read (sym)\n");
					goto beach;
				}
#if R_BIN_ELF64
				sym[j].st_name = READ32 (s, k)
				sym[j].st_info = READ8 (s, k)
				sym[j].st_other = READ8 (s, k)
				sym[j].st_shndx = READ16 (s, k)
				sym[j].st_value = READ64 (s, k)
				sym[j].st_size = READ64 (s, k)
#else
				sym[j].st_name = READ32 (s, k)
				sym[j].st_value = READ32 (s, k)
				sym[j].st_size = READ32 (s, k)
				sym[j].st_info = READ8 (s, k)
				sym[j].st_other = READ8 (s, k)
				sym[j].st_shndx = READ16 (s, k)
#endif
			}
			free (ret);
			ret = calloc (nsym, sizeof (RBinElfSymbol));
			if (!ret) {
				bprintf ("Cannot allocate %d symbols\n", nsym);
				goto beach;
			}
			for (k = 1, ret_ctr = 0; k < nsym; k++) {
				if (type == R_BIN_ELF_IMPORTS && sym[k].st_shndx == STN_UNDEF) {
					if (sym[k].st_value) {
						toffset = sym[k].st_value;
					} else if ((toffset = get_import_addr (bin, k)) == -1){
						toffset = 0;
					}
					tsize = 16;
				} else if (type == R_BIN_ELF_SYMBOLS &&
					   sym[k].st_shndx != STN_UNDEF &&
					   ELF_ST_TYPE (sym[k].st_info) != STT_SECTION &&
					   ELF_ST_TYPE (sym[k].st_info) != STT_FILE) {
					tsize = sym[k].st_size;
					toffset = (ut64)sym[k].st_value; 
				} else {
					continue;
				}
				if (bin->ehdr.e_type == ET_REL) {
					if (sym[k].st_shndx < bin->ehdr.e_shnum)
						ret[ret_ctr].offset = sym[k].st_value + bin->shdr[sym[k].st_shndx].sh_offset;
				} else {
					ret[ret_ctr].offset = Elf_(r_bin_elf_v2p) (bin, toffset);
				}
				ret[ret_ctr].size = tsize;
				if (sym[k].st_name + 2 > strtab_section->sh_size) {
					bprintf ("Warning: index out of strtab range\n");
					goto beach;
				}
				{
					int rest = ELF_STRING_LENGTH - 1;
					int st_name = sym[k].st_name;
					int maxsize = R_MIN (bin->b->length, strtab_section->sh_size);
					if (st_name < 0 || st_name >= maxsize) {
						ret[ret_ctr].name[0] = 0;
					} else {
						const size_t len = __strnlen (strtab + sym[k].st_name, rest);
						memcpy (ret[ret_ctr].name, &strtab[sym[k].st_name], len);
					}
				}
				ret[ret_ctr].ordinal = k;
				ret[ret_ctr].name[ELF_STRING_LENGTH - 2] = '\0';
				fill_symbol_bind_and_type (&ret[ret_ctr], &sym[k]);
				ret[ret_ctr].last = 0;
				ret_ctr++;
			}
			ret[ret_ctr].last = 1; // ugly dirty hack :D
			R_FREE (strtab);
			R_FREE (sym);
		}
	}
	if (!ret) {
		return (type == R_BIN_ELF_SYMBOLS)
				? Elf_(r_bin_elf_get_phdr_symbols) (bin)
				: Elf_(r_bin_elf_get_phdr_imports) (bin);
	}
	int max = -1;
	RBinElfSymbol *aux = NULL;
	nsym = Elf_(fix_symbols) (bin, ret_ctr, type, &ret);
	if (nsym == -1) {
		goto beach;
	}
	aux = ret;
	while (!aux->last) {
		if ((int)aux->ordinal > max) {
			max = aux->ordinal;
		}
		aux++;
	}
	nsym = max;
	if (type == R_BIN_ELF_IMPORTS) {
		R_FREE (bin->imports_by_ord);
		bin->imports_by_ord_size = nsym + 1;
		bin->imports_by_ord = (RBinImport**)calloc (R_MAX (1, nsym + 1), sizeof (RBinImport*));
	} else if (type == R_BIN_ELF_SYMBOLS) {
		R_FREE (bin->symbols_by_ord);
		bin->symbols_by_ord_size = nsym + 1;
		bin->symbols_by_ord = (RBinSymbol**)calloc (R_MAX (1, nsym + 1), sizeof (RBinSymbol*));
	}
	return ret;
beach:
	free (ret);
	free (sym);
	free (strtab);
	return NULL;
}
