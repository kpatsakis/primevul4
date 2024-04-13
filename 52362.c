static u64 fixed_mtrr_seg_unit_size(int seg)
{
	return 8 << fixed_seg_table[seg].range_shift;
}
