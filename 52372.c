static void mtrr_lookup_fixed_next(struct mtrr_iter *iter)
{
	/* terminate the lookup. */
	if (fixed_mtrr_range_end_addr(iter->seg, iter->index) >= iter->end) {
		iter->fixed = false;
		iter->range = NULL;
		return;
	}

	iter->index++;

	/* have looked up for all fixed MTRRs. */
	if (iter->index >= ARRAY_SIZE(iter->mtrr_state->fixed_ranges))
		return mtrr_lookup_var_start(iter);

	/* switch to next segment. */
	if (iter->index > fixed_mtrr_seg_end_range_index(iter->seg))
		iter->seg++;
}
