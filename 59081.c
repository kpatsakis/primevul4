static RList *entries(RBinFile *arch) {
	RListIter *iter;
	RBinDexObj *bin;
	RBinSymbol *m;
	RBinAddr *ptr;
	RList *ret;

	if (!arch || !arch->o || !arch->o->bin_obj) {
		return NULL;
	}
	bin = (RBinDexObj*) arch->o->bin_obj;
	ret = r_list_newf ((RListFree)free);

	if (!bin->methods_list) {
		dex_loadcode (arch, bin);
	}

	r_list_foreach (bin->methods_list, iter, m) {
		if (strlen (m->name) > 30 && m->bind && 
			!strcmp(m->bind, "GLOBAL") &&
		    !strcmp (m->name + strlen (m->name) - 31,
			     ".onCreate(Landroid/os/Bundle;)V")) {
			if (!already_entry (ret, m->paddr)) {
				if ((ptr = R_NEW0 (RBinAddr))) {
					ptr->paddr = ptr->vaddr = m->paddr;
					r_list_append (ret, ptr);
				}
			}
		}
	}

	if (r_list_empty (ret)) {
		r_list_foreach (bin->methods_list, iter, m) {
			if (strlen (m->name) > 26 &&
			    !strcmp (m->name + strlen (m->name) - 27,
				     ".main([Ljava/lang/String;)V")) {
				if (!already_entry (ret, m->paddr)) {
					if ((ptr = R_NEW0 (RBinAddr))) {
						ptr->paddr = ptr->vaddr = m->paddr;
						r_list_append (ret, ptr);
					}
				}
			}
		}		
	}

	if (r_list_empty (ret)) {
		if (!already_entry (ret, bin->code_from)) {
			ptr = R_NEW0 (RBinAddr);
			if (ptr) {
				ptr->paddr = ptr->vaddr = bin->code_from;
				r_list_append (ret, ptr);
			}
		}
	}
	return ret;
}
