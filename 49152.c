static void __exit camellia_sparc64_mod_fini(void)
{
	crypto_unregister_algs(algs, ARRAY_SIZE(algs));
}
