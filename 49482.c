static int cprng_reset(struct crypto_rng *tfm, u8 *seed, unsigned int slen)
{
	struct prng_context *prng = crypto_rng_ctx(tfm);
	u8 *key = seed + DEFAULT_BLK_SZ;
	u8 *dt = NULL;

	if (slen < DEFAULT_PRNG_KSZ + DEFAULT_BLK_SZ)
		return -EINVAL;

	if (slen >= (2 * DEFAULT_BLK_SZ + DEFAULT_PRNG_KSZ))
		dt = key + DEFAULT_PRNG_KSZ;

	reset_prng_context(prng, key, DEFAULT_PRNG_KSZ, seed, dt);

	if (prng->flags & PRNG_NEED_RESET)
		return -EINVAL;
	return 0;
}
