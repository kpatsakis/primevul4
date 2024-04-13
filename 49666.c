static void __exit rmd256_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
