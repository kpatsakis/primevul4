static int crc32c_sparc64_digest(struct shash_desc *desc, const u8 *data,
				 unsigned int len, u8 *out)
{
	return __crc32c_sparc64_finup(crypto_shash_ctx(desc->tfm), data, len,
				      out);
}
