static void __exit rmd128_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
