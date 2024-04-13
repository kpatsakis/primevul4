static void __exit ghash_pclmulqdqni_mod_exit(void)
{
	crypto_unregister_ahash(&ghash_async_alg);
	crypto_unregister_shash(&ghash_alg);
}
