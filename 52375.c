static void mtrr_lookup_next(struct mtrr_iter *iter)
{
	if (iter->fixed)
		mtrr_lookup_fixed_next(iter);
	else
		mtrr_lookup_var_next(iter);
}
