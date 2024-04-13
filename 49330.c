static int crc32c_intel_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return __crc32c_intel_finup(shash_desc_ctx(desc), data, len, out);
}
