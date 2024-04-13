static int cprng_get_random(struct crypto_rng *tfm, u8 *rdata,
			    unsigned int dlen)
{
	struct prng_context *prng = crypto_rng_ctx(tfm);

	return get_prng_bytes(rdata, dlen, prng, 0);
}
