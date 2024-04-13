static void __exit sha256_generic_mod_fini(void)
{
	crypto_unregister_shashes(sha256_algs, ARRAY_SIZE(sha256_algs));
}
