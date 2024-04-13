static int crc32c_intel_final(struct shash_desc *desc, u8 *out)
{
	u32 *crcp = shash_desc_ctx(desc);

	*(__le32 *)out = ~cpu_to_le32p(crcp);
	return 0;
}
