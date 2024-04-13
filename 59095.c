static RList *sections(RBinFile *arch) {
	struct r_bin_dex_obj_t *bin = arch->o->bin_obj;
	RList *ml = methods (arch);
	RBinSection *ptr = NULL;
	int ns, fsymsz = 0;
	RList *ret = NULL;
	RListIter *iter;
	RBinSymbol *m;
	int fsym = 0;

	r_list_foreach (ml, iter, m) {
		if (!fsym || m->paddr < fsym) {
			fsym = m->paddr;
		}
		ns = m->paddr + m->size;
		if (ns > arch->buf->length) {
			continue;
		}
		if (ns > fsymsz) {
			fsymsz = ns;
		}
	}
	if (!fsym) {
		return NULL;
	}
	if (!(ret = r_list_new ())) {
		return NULL;
	}
	ret->free = free;

	if ((ptr = R_NEW0 (RBinSection))) {
		strcpy (ptr->name, "header");
		ptr->size = ptr->vsize = sizeof (struct dex_header_t);
		ptr->paddr= ptr->vaddr = 0;
		ptr->srwx = R_BIN_SCN_READABLE | R_BIN_SCN_MAP;
		ptr->add = true;
		r_list_append (ret, ptr);
	}
	if ((ptr = R_NEW0 (RBinSection))) {
		strcpy (ptr->name, "constpool");
		ptr->paddr= ptr->vaddr = sizeof (struct dex_header_t);
		ptr->size = bin->code_from - ptr->vaddr; // fix size
		ptr->srwx = R_BIN_SCN_READABLE | R_BIN_SCN_MAP;
		ptr->add = true;
		r_list_append (ret, ptr);
	}
	if ((ptr = R_NEW0 (RBinSection))) {
		strcpy (ptr->name, "code");
		ptr->vaddr = ptr->paddr = bin->code_from; //ptr->vaddr = fsym;
		ptr->size = bin->code_to - ptr->paddr;
		ptr->srwx = R_BIN_SCN_READABLE | R_BIN_SCN_EXECUTABLE | R_BIN_SCN_MAP;
		ptr->add = true;
		r_list_append (ret, ptr);
	}
	if ((ptr = R_NEW0 (RBinSection))) {
		strcpy (ptr->name, "data");
		ptr->paddr = ptr->vaddr = fsymsz+fsym;
		if (ptr->vaddr > arch->buf->length) {
			ptr->paddr = ptr->vaddr = bin->code_to;
			ptr->size = ptr->vsize = arch->buf->length - ptr->vaddr;
		} else {
			ptr->size = ptr->vsize = arch->buf->length - ptr->vaddr;
		}
		ptr->srwx = R_BIN_SCN_READABLE | R_BIN_SCN_MAP; //|2;
		ptr->add = true;
		r_list_append (ret, ptr);
	}
	return ret;
}
