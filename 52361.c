static int fixed_mtrr_seg_unit_range_index(int seg, int unit)
{
	struct fixed_mtrr_segment *mtrr_seg = &fixed_seg_table[seg];

	WARN_ON(mtrr_seg->start + unit * fixed_mtrr_seg_unit_size(seg)
		> mtrr_seg->end);

	/* each unit has 8 ranges. */
	return mtrr_seg->range_start + 8 * unit;
}
