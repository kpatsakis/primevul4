static void __exit rmd320_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}
