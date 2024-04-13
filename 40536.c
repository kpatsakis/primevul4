struct shash_alg *shash_attr_alg(struct rtattr *rta, u32 type, u32 mask)
{
	struct crypto_alg *alg;

	alg = crypto_attr_alg2(rta, &crypto_shash_type, type, mask);
	return IS_ERR(alg) ? ERR_CAST(alg) :
	       container_of(alg, struct shash_alg, base);
}
