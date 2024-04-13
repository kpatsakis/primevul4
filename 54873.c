static RBinElfSymbol* get_symbols_from_phdr(ELFOBJ *bin, int type) {
	Elf_(Sym) *sym = NULL;
	Elf_(Addr) addr_sym_table = 0;
	ut8 s[sizeof (Elf_(Sym))] = {0};
	RBinElfSymbol *ret = NULL;
	int i, j, r, tsize, nsym, ret_ctr;
	ut64 toffset = 0, tmp_offset;
	ut32 size, sym_size = 0;

	if (!bin || !bin->phdr || !bin->ehdr.e_phnum) {
		return NULL;
	}
	for (j = 0; j < bin->dyn_entries; j++) {
		switch (bin->dyn_buf[j].d_tag) {
		case (DT_SYMTAB):
			addr_sym_table = Elf_(r_bin_elf_v2p) (bin, bin->dyn_buf[j].d_un.d_ptr);
			break;
		case (DT_SYMENT):
			sym_size = bin->dyn_buf[j].d_un.d_val;
			break;
		default:
			break;
		}
	}
	if (!addr_sym_table) {
		return NULL;
	}
	if (!sym_size) {
		return NULL;
	}
	nsym = (bin->size - addr_sym_table) / sym_size;
	if (!UT32_MUL (&size, nsym, sizeof (Elf_ (Sym)))) {
		goto beach;
	}
	if (size < 1) {
		goto beach;
	}
	if (addr_sym_table > bin->size || addr_sym_table + size > bin->size) {
		goto beach;
	}
	if (nsym < 1) {
		return NULL;
	}
	size_t capacity1 = 4096;
	size_t capacity2 = 4096;
	sym = (Elf_(Sym)*) calloc (capacity1, sym_size);
	ret = (RBinElfSymbol *) calloc (capacity2, sizeof (struct r_bin_elf_symbol_t));
	if (!sym || !ret) {
		goto beach;
	}
	for (i = 1, ret_ctr = 0; i < nsym; i++) {
		if (i >= capacity1) { // maybe grow
			Elf_(Sym)* temp_sym = (Elf_(Sym)*) realloc(sym, (capacity1 * GROWTH_FACTOR) * sym_size);
			if (!temp_sym) {
				goto beach;
			}
			sym = temp_sym;
			capacity1 *= GROWTH_FACTOR;
		}
		if (ret_ctr >= capacity2) { // maybe grow
			RBinElfSymbol *temp_ret = realloc (ret, capacity2 * GROWTH_FACTOR * sizeof (struct r_bin_elf_symbol_t));
			if (!temp_ret) {
				goto beach;
			}
			ret = temp_ret;
			capacity2 *= GROWTH_FACTOR;
		}
		r = r_buf_read_at (bin->b, addr_sym_table + i * sizeof (Elf_ (Sym)), s, sizeof (Elf_ (Sym)));
		if (r < 1) {
			goto beach;
		}
		int j = 0;
#if R_BIN_ELF64
		sym[i].st_name = READ32 (s, j);
		sym[i].st_info = READ8 (s, j);
		sym[i].st_other = READ8 (s, j);
		sym[i].st_shndx = READ16 (s, j);
		sym[i].st_value = READ64 (s, j);
		sym[i].st_size = READ64 (s, j);
#else
		sym[i].st_name = READ32 (s, j);
		sym[i].st_value = READ32 (s, j);
		sym[i].st_size = READ32 (s, j);
		sym[i].st_info = READ8 (s, j);
		sym[i].st_other = READ8 (s, j);
		sym[i].st_shndx = READ16 (s, j);
#endif
		if (type == R_BIN_ELF_IMPORTS && sym[i].st_shndx == STN_UNDEF) {
			if (sym[i].st_value) {
				toffset = sym[i].st_value;
			} else if ((toffset = get_import_addr (bin, i)) == -1){
				toffset = 0;
			}
			tsize = 16;
		} else if (type == R_BIN_ELF_SYMBOLS &&
		           sym[i].st_shndx != STN_UNDEF &&
		           ELF_ST_TYPE (sym[i].st_info) != STT_SECTION &&
		           ELF_ST_TYPE (sym[i].st_info) != STT_FILE) {
			tsize = sym[i].st_size;
			toffset = (ut64) sym[i].st_value;
		} else {
			continue;
		}
		tmp_offset = Elf_(r_bin_elf_v2p) (bin, toffset);
		if (tmp_offset > bin->size) {
			goto done;
		}
		if (sym[i].st_name + 2 > bin->strtab_size) {
			goto done;
		}
		ret[ret_ctr].offset = tmp_offset;
		ret[ret_ctr].size = tsize;
		{
			int rest = ELF_STRING_LENGTH - 1;
			int st_name = sym[i].st_name;
			int maxsize = R_MIN (bin->size, bin->strtab_size);
			if (st_name < 0 || st_name >= maxsize) {
				ret[ret_ctr].name[0] = 0;
			} else {
				const int len = __strnlen (bin->strtab + st_name, rest);
				memcpy (ret[ret_ctr].name, &bin->strtab[st_name], len);
			}
		}
		ret[ret_ctr].ordinal = i;
		ret[ret_ctr].in_shdr = false;
		ret[ret_ctr].name[ELF_STRING_LENGTH - 2] = '\0';
		fill_symbol_bind_and_type (&ret[ret_ctr], &sym[i]);
		ret[ret_ctr].last = 0;
		ret_ctr++;
	}
done:
	ret[ret_ctr].last = 1;
	{
		nsym = i > 0 ? i : 1;
		Elf_ (Sym) * temp_sym = (Elf_ (Sym)*) realloc (sym, (nsym * GROWTH_FACTOR) * sym_size);
		if (!temp_sym) {
			goto beach;
		}
		sym = temp_sym;
	}
	{
		ret_ctr = ret_ctr > 0 ? ret_ctr : 1;
		RBinElfSymbol *p = (RBinElfSymbol *) realloc (ret, (ret_ctr + 1) * sizeof (RBinElfSymbol));
		if (!p) {
			goto beach;
		}
		ret = p;
	}
	if (type == R_BIN_ELF_IMPORTS && !bin->imports_by_ord_size) {
		bin->imports_by_ord_size = ret_ctr + 1;
		if (ret_ctr > 0) {
			bin->imports_by_ord = (RBinImport * *) calloc (ret_ctr + 1, sizeof (RBinImport*));
		} else {
			bin->imports_by_ord = NULL;
		}
	} else if (type == R_BIN_ELF_SYMBOLS && !bin->symbols_by_ord_size && ret_ctr) {
		bin->symbols_by_ord_size = ret_ctr + 1;
		if (ret_ctr > 0) {
			bin->symbols_by_ord = (RBinSymbol * *) calloc (ret_ctr + 1, sizeof (RBinSymbol*));
		}else {
			bin->symbols_by_ord = NULL;
		}
	}
	free (sym);
	return ret;
beach:
	free (sym);
	free (ret);
	return NULL;
}
