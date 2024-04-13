R_API RFlagItem *r_flag_set(RFlag *f, const char *name, ut64 off, ut32 size) {
	RFlagItem *item = NULL;
	RList *list;

	/* contract fail */
	if (!name || !*name) {
		return NULL;
	}

	item = r_flag_get (f, name);
	if (item) {
		if (item->offset == off) {
			item->size = size;
			return item;
		}
		remove_offsetmap (f, item);
	} else {
		item = R_NEW0 (RFlagItem);
		if (!item) {
			return NULL;
		}
		if (!set_name (item, name)) {
			eprintf ("Invalid flag name '%s'.\n", name);
			r_flag_item_free (item);
			return NULL;
		}
		ht_insert (f->ht_name, item->name, item);
		r_list_append (f->flags, item);
	}

	item->space = f->space_idx;
	item->offset = off + f->base;
	item->size = size;

	list = (RList *)r_flag_get_list (f, off);
	if (!list) {
		RFlagsAtOffset *flagsAtOffset = R_NEW (RFlagsAtOffset);
		list = r_list_new ();
		flagsAtOffset->flags = list;
		flagsAtOffset->off = off;
		r_skiplist_insert (f->by_off, flagsAtOffset);
	}
	r_list_append (list, item);
	return item;
}
