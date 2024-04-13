static int __crc32c_pcl_intel_finup(u32 *crcp, const u8 *data, unsigned int len,
				u8 *out)
{
	if (len >= crc32c_pcl_breakeven && irq_fpu_usable()) {
		kernel_fpu_begin();
		*(__le32 *)out = ~cpu_to_le32(crc_pcl(data, len, *crcp));
		kernel_fpu_end();
	} else
		*(__le32 *)out =
			~cpu_to_le32(crc32c_intel_le_hw(*crcp, data, len));
	return 0;
}
