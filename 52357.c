static int fixed_mtrr_addr_seg_to_range_index(u64 addr, int seg)
{
	struct fixed_mtrr_segment *mtrr_seg;
	int index;

	mtrr_seg = &fixed_seg_table[seg];
	index = mtrr_seg->range_start;
	index += (addr - mtrr_seg->start) >> mtrr_seg->range_shift;
	return index;
}
