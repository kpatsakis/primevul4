static int fips_cprng_reset(struct crypto_rng *tfm, u8 *seed, unsigned int slen)
{
	u8 rdata[DEFAULT_BLK_SZ];
	u8 *key = seed + DEFAULT_BLK_SZ;
	int rc;

	struct prng_context *prng = crypto_rng_ctx(tfm);

	if (slen < DEFAULT_PRNG_KSZ + DEFAULT_BLK_SZ)
		return -EINVAL;

	/* fips strictly requires seed != key */
	if (!memcmp(seed, key, DEFAULT_PRNG_KSZ))
		return -EINVAL;

	rc = cprng_reset(tfm, seed, slen);

	if (!rc)
		goto out;

	/* this primes our continuity test */
	rc = get_prng_bytes(rdata, DEFAULT_BLK_SZ, prng, 0);
	prng->rand_data_valid = DEFAULT_BLK_SZ;

out:
	return rc;
}
