static int fixed_mtrr_seg_end_range_index(int seg)
{
	struct fixed_mtrr_segment *mtrr_seg = &fixed_seg_table[seg];
	int n;

	n = (mtrr_seg->end - mtrr_seg->start) >> mtrr_seg->range_shift;
	return mtrr_seg->range_start + n - 1;
}
