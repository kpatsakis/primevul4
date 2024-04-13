static void __implement(u8 *report, unsigned offset, int n, u32 value)
{
	unsigned int idx = offset / 8;
	unsigned int size = offset + n;
	unsigned int bit_shift = offset % 8;
	int bits_to_set = 8 - bit_shift;
	u8 bit_mask = 0xff << bit_shift;

	while (n - bits_to_set >= 0) {
		report[idx] &= ~bit_mask;
		report[idx] |= value << bit_shift;
		value >>= bits_to_set;
		n -= bits_to_set;
		bits_to_set = 8;
		bit_mask = 0xff;
		bit_shift = 0;
		idx++;
	}

	/* last nibble */
	if (n) {
		if (size % 8)
			bit_mask &= (1U << (size % 8)) - 1;
		report[idx] &= ~bit_mask;
		report[idx] |= (value << bit_shift) & bit_mask;
	}
}
