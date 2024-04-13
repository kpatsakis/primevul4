static bool match_var_range(struct mtrr_iter *iter,
			    struct kvm_mtrr_range *range)
{
	u64 start, end;

	var_mtrr_range(range, &start, &end);
	if (!(start >= iter->end || end <= iter->start)) {
		iter->range = range;

		/*
		 * the function is called when we do kvm_mtrr.head walking.
		 * Range has the minimum base address which interleaves
		 * [looker->start_max, looker->end).
		 */
		iter->partial_map |= iter->start_max < start;

		/* update the max address has been covered. */
		iter->start_max = max(iter->start_max, end);
		return true;
	}

	return false;
}
