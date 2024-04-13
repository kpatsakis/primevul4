static int crypto_shash_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_shash *hash = __crypto_shash_cast(tfm);

	hash->descsize = crypto_shash_alg(hash)->descsize;
	return 0;
}
