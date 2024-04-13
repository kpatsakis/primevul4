static bool mtrr_lookup_fixed_start(struct mtrr_iter *iter)
{
	int seg, index;

	if (!fixed_mtrr_is_enabled(iter->mtrr_state))
		return false;

	seg = fixed_mtrr_addr_to_seg(iter->start);
	if (seg < 0)
		return false;

	iter->fixed = true;
	index = fixed_mtrr_addr_seg_to_range_index(iter->start, seg);
	iter->index = index;
	iter->seg = seg;
	return true;
}
