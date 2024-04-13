int crypto_shash_digest(struct shash_desc *desc, const u8 *data,
			unsigned int len, u8 *out)
{
	struct crypto_shash *tfm = desc->tfm;
	struct shash_alg *shash = crypto_shash_alg(tfm);
	unsigned long alignmask = crypto_shash_alignmask(tfm);

	if (((unsigned long)data | (unsigned long)out) & alignmask)
		return shash_digest_unaligned(desc, data, len, out);

	return shash->digest(desc, data, len, out);
}
