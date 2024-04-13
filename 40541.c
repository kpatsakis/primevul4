static int shash_finup_unaligned(struct shash_desc *desc, const u8 *data,
				 unsigned int len, u8 *out)
{
	return crypto_shash_update(desc, data, len) ?:
	       crypto_shash_final(desc, out);
}
