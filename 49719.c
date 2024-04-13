static void __exit sha512_generic_mod_fini(void)
{
	crypto_unregister_shashes(sha512_algs, ARRAY_SIZE(sha512_algs));
}
