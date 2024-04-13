static int __crc32c_intel_finup(u32 *crcp, const u8 *data, unsigned int len,
				u8 *out)
{
	*(__le32 *)out = ~cpu_to_le32(crc32c_intel_le_hw(*crcp, data, len));
	return 0;
}
