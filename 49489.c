static int __init prng_mod_init(void)
{
	return crypto_register_algs(rng_algs, ARRAY_SIZE(rng_algs));
}
