static RList *patch_relocs(RBin *b) {
	struct r_bin_bflt_obj *bin = NULL;
	RList *list = NULL;
	RBinObject *obj;
	int i = 0;
	if (!b || !b->iob.io || !b->iob.io->desc) {
		return NULL;
	}
	if (!b->iob.io->cached) {
		eprintf (
			"Warning: please run r2 with -e io.cache=true to patch "
			"relocations\n");
		return list;
	}
	
	obj = r_bin_cur_object (b);
	if (!obj) {
		return NULL;
	}
	bin = obj->bin_obj;
	list = r_list_newf ((RListFree)free);
	if (!list) {
		return NULL;
	}
	if (bin->got_table) {
		struct reloc_struct_t *got_table = bin->got_table;
		for (i = 0; i < bin->n_got; i++) {
			__patch_reloc (bin->b, got_table[i].addr_to_patch,
				       got_table[i].data_offset);
			RBinReloc *reloc = R_NEW0 (RBinReloc);
			if (reloc) {
				reloc->type = R_BIN_RELOC_32;
				reloc->paddr = got_table[i].addr_to_patch;
				reloc->vaddr = reloc->paddr;
				r_list_append (list, reloc);
			}
		}
		R_FREE (bin->got_table);
	}

	if (bin->reloc_table) {
		struct reloc_struct_t *reloc_table = bin->reloc_table;
		for (i = 0; i < bin->hdr->reloc_count; i++) {
			int found = search_old_relocation (reloc_table, 
						reloc_table[i].addr_to_patch, 
						bin->hdr->reloc_count);
			if (found != -1) {
				__patch_reloc (bin->b, reloc_table[found].addr_to_patch, 
							reloc_table[i].data_offset);
			} else {
				__patch_reloc (bin->b, reloc_table[i].addr_to_patch,
							reloc_table[i].data_offset);
			}
			RBinReloc *reloc = R_NEW0 (RBinReloc);
			if (reloc) {
				reloc->type = R_BIN_RELOC_32;
				reloc->paddr = reloc_table[i].addr_to_patch;
				reloc->vaddr = reloc->paddr;
				r_list_append (list, reloc);
			}
		}
		R_FREE (bin->reloc_table);
	}
	b->iob.write_at (b->iob.io, bin->b->base, bin->b->buf, bin->b->length);
	return list;
}
