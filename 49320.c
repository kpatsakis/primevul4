static int crc32_pclmul_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return __crc32_pclmul_finup(shash_desc_ctx(desc), data, len, out);
}
