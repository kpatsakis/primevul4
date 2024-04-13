static int crc32_update(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	u32 *crcp = shash_desc_ctx(desc);

	*crcp = __crc32_le(*crcp, data, len);
	return 0;
}
