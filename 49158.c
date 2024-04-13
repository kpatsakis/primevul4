static int __crc32c_sparc64_finup(u32 *crcp, const u8 *data, unsigned int len,
				  u8 *out)
{
	u32 tmp = *crcp;

	crc32c_compute(&tmp, (const u64 *) data, len);

	*(__le32 *) out = ~cpu_to_le32(tmp);
	return 0;
}
