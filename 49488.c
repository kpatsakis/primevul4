static void __exit prng_mod_fini(void)
{
	crypto_unregister_algs(rng_algs, ARRAY_SIZE(rng_algs));
}
