static int tg3_do_mem_test(struct tg3 *tp, u32 offset, u32 len)
{
	static const u32 test_pattern[] = { 0x00000000, 0xffffffff, 0xaa55a55a };
	int i;
	u32 j;

	for (i = 0; i < ARRAY_SIZE(test_pattern); i++) {
		for (j = 0; j < len; j += 4) {
			u32 val;

			tg3_write_mem(tp, offset + j, test_pattern[i]);
			tg3_read_mem(tp, offset + j, &val);
			if (val != test_pattern[i])
				return -EIO;
		}
	}
	return 0;
}
