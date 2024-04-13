static int fixed_mtrr_addr_to_seg(u64 addr)
{
	struct fixed_mtrr_segment *mtrr_seg;
	int seg, seg_num = ARRAY_SIZE(fixed_seg_table);

	for (seg = 0; seg < seg_num; seg++) {
		mtrr_seg = &fixed_seg_table[seg];
		if (mtrr_seg->start <= addr && addr < mtrr_seg->end)
			return seg;
	}

	return -1;
}
