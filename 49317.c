static int __crc32_pclmul_finup(u32 *crcp, const u8 *data, unsigned int len,
				u8 *out)
{
	*(__le32 *)out = cpu_to_le32(crc32_pclmul_le(*crcp, data, len));
	return 0;
}
