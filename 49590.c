static void __exit deflate_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
