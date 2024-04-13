R_API int r_flag_relocate(RFlag *f, ut64 off, ut64 off_mask, ut64 to) {
	ut64 neg_mask = ~(off_mask);
	RFlagItem *item;
	RListIter *iter;
	int n = 0;

	r_list_foreach (f->flags, iter, item) {
		ut64 fn = item->offset & neg_mask;
		ut64 on = off & neg_mask;
		if (fn == on) {
			ut64 fm = item->offset & off_mask;
			ut64 om = to & off_mask;
			item->offset = (to&neg_mask) + fm + om;
			n++;
		}
	}
	return n;
}
