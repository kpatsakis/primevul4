R_API RFlagItem *r_flag_get_i2(RFlag *f, ut64 off) {
	RFlagItem *oitem = NULL, *item = NULL;
	RListIter *iter;
	const RList *list = r_flag_get_list (f, off);
	if (!list) {
		return NULL;
	}
	r_list_foreach (list, iter, item) {
		if (!item->name) {
			continue;
		}
		/* catch sym. first */
		if (!strncmp (item->name, "loc.", 4)) {
			continue;
		}
		if (!strncmp (item->name, "fcn.", 4)) {
			continue;
		}
		if (!strncmp (item->name, "section.", 8)) {
			continue;
		}
		if (!strncmp (item->name, "section_end.", 12)) {
			continue;
		}
		if (r_str_nlen (item->name, 5) > 4 &&
		    item->name[3] == '.') {
			oitem = item;
			break;
		}
		oitem = item;
		if (strlen (item->name) < 5 || item->name[3]!='.') continue;
		oitem = item;
	}
	return evalFlag (f, oitem);
}
