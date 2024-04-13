static bool mtrr_lookup_okay(struct mtrr_iter *iter)
{
	if (iter->fixed) {
		iter->mem_type = iter->mtrr_state->fixed_ranges[iter->index];
		return true;
	}

	if (iter->range) {
		iter->mem_type = iter->range->base & 0xff;
		return true;
	}

	return false;
}
