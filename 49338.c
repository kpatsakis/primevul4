static int crc32c_pcl_intel_digest(struct shash_desc *desc, const u8 *data,
			       unsigned int len, u8 *out)
{
	return __crc32c_pcl_intel_finup(crypto_shash_ctx(desc->tfm), data, len,
				    out);
}
