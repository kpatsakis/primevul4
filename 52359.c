static u64 fixed_mtrr_range_end_addr(int seg, int index)
{
	struct fixed_mtrr_segment *mtrr_seg = &fixed_seg_table[seg];
	int pos = index - mtrr_seg->range_start;

	return mtrr_seg->start + ((pos + 1) << mtrr_seg->range_shift);
}
