static unsigned int crypto_skcipher_extsize(struct crypto_alg *alg)
{
	if (alg->cra_type == &crypto_blkcipher_type)
		return sizeof(struct crypto_blkcipher *);

	if (alg->cra_type == &crypto_ablkcipher_type ||
	    alg->cra_type == &crypto_givcipher_type)
		return sizeof(struct crypto_ablkcipher *);

	return crypto_alg_extsize(alg);
}
