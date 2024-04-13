struct hash_alg_common *ahash_attr_alg(struct rtattr *rta, u32 type, u32 mask)
{
	struct crypto_alg *alg;

	alg = crypto_attr_alg2(rta, &crypto_ahash_type, type, mask);
	return IS_ERR(alg) ? ERR_CAST(alg) : __crypto_hash_alg_common(alg);
}
