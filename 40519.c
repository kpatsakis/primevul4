int crypto_shash_final(struct shash_desc *desc, u8 *out)
{
	struct crypto_shash *tfm = desc->tfm;
	struct shash_alg *shash = crypto_shash_alg(tfm);
	unsigned long alignmask = crypto_shash_alignmask(tfm);

	if ((unsigned long)out & alignmask)
		return shash_final_unaligned(desc, out);

	return shash->final(desc, out);
}
