R_API RFlagItem *r_flag_get_at(RFlag *f, ut64 off, bool closest) {
	RFlagItem *item, *nice = NULL;
	RListIter *iter;
	const RFlagsAtOffset *flags_at = r_flag_get_nearest_list (f, off, -1);
	if (!flags_at) {
		return NULL;
	}
	if (flags_at->off == off) {
		r_list_foreach (flags_at->flags, iter, item) {
			if (f->space_idx != -1 && item->space != f->space_idx) {
				continue;
			}
			if (nice) {
				if (isFunctionFlag (nice->name)) {
					nice = item;
				}
			} else {
				nice = item;
			}
		}
		return nice;
	}

	if (!closest) {
		return NULL;
	}
	while (!nice && flags_at) {
		r_list_foreach (flags_at->flags, iter, item) {
			if (f->space_strict && IS_IN_SPACE (f, item)) {
				continue;
			}
			if (item->offset == off) {
				eprintf ("XXX Should never happend\n");
				return evalFlag (f, item);
			}
			nice = item;
			break;
		}
		if (flags_at->off) {
			flags_at = r_flag_get_nearest_list (f, flags_at->off - 1, -1);
		} else {
			flags_at = NULL;
		}
	}
	return evalFlag (f, nice);
}
