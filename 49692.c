static void __exit serpent_mod_fini(void)
{
	crypto_unregister_algs(srp_algs, ARRAY_SIZE(srp_algs));
}
