static unsigned int crypto_ahash_extsize(struct crypto_alg *alg)
{
	if (alg->cra_type == &crypto_ahash_type)
		return alg->cra_ctxsize;

	return sizeof(struct crypto_shash *);
}
