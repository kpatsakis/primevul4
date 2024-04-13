static int fixed_msr_to_range_index(u32 msr)
{
	int seg, unit;

	if (!fixed_msr_to_seg_unit(msr, &seg, &unit))
		return -1;

	return fixed_mtrr_seg_unit_range_index(seg, unit);
}
