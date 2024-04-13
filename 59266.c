perf_output_sample_regs(struct perf_output_handle *handle,
			struct pt_regs *regs, u64 mask)
{
	int bit;
	DECLARE_BITMAP(_mask, 64);

	bitmap_from_u64(_mask, mask);
	for_each_set_bit(bit, _mask, sizeof(mask) * BITS_PER_BYTE) {
		u64 val;

		val = perf_reg_value(regs, bit);
		perf_output_put(handle, val);
	}
}
