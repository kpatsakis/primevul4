static RFlagItem *evalFlag(RFlag *f, RFlagItem *item) {
	if (item && item->alias) {
		item->offset = r_num_math (f->num, item->alias);
	}
	return item;
}
