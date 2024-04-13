static int crc32_pclmul_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len)
{
	u32 *crcp = shash_desc_ctx(desc);

	*crcp = crc32_pclmul_le(*crcp, data, len);
	return 0;
}
