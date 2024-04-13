static void mtrr_lookup_start(struct mtrr_iter *iter)
{
	if (!mtrr_is_enabled(iter->mtrr_state)) {
		iter->mtrr_disabled = true;
		return;
	}

	if (!mtrr_lookup_fixed_start(iter))
		mtrr_lookup_var_start(iter);
}
