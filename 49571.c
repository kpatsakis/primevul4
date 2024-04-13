static void __exit crypto_null_mod_fini(void)
{
	crypto_unregister_shash(&digest_null);
	crypto_unregister_algs(null_algs, ARRAY_SIZE(null_algs));
}
