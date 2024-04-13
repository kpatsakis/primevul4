static int krng_get_random(struct crypto_rng *tfm, u8 *rdata, unsigned int dlen)
{
	get_random_bytes(rdata, dlen);
	return 0;
}
