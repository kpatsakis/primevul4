static int __chksum_finup(u32 *crcp, const u8 *data, unsigned int len, u8 *out)
{
	*(__le32 *)out = ~cpu_to_le32(__crc32c_le(*crcp, data, len));
	return 0;
}
