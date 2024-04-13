static void __exit sha256_ssse3_mod_fini(void)
{
	crypto_unregister_shashes(algs, ARRAY_SIZE(algs));
}
