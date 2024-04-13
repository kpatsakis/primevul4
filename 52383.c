static bool var_mtrr_range_is_valid(struct kvm_mtrr_range *range)
{
	return (range->mask & (1 << 11)) != 0;
}
