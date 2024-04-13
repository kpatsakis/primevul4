static int valid_csum(struct ti_i2c_desc *rom_desc, __u8 *buffer)
{
	__u16 i;
	__u8 cs = 0;

	for (i = 0; i < le16_to_cpu(rom_desc->Size); i++)
		cs = (__u8)(cs + buffer[i]);

	if (cs != rom_desc->CheckSum) {
		pr_debug("%s - Mismatch %x - %x", __func__, rom_desc->CheckSum, cs);
		return -EINVAL;
	}
	return 0;
}
