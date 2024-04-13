static void __exit sha512_neon_mod_fini(void)
{
	crypto_unregister_shashes(algs, ARRAY_SIZE(algs));
}
