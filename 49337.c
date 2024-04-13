static int crc32c_intel_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len)
{
	u32 *crcp = shash_desc_ctx(desc);

	*crcp = crc32c_intel_le_hw(*crcp, data, len);
	return 0;
}
