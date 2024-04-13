static void __exit nx842_mod_exit(void)
{
	crypto_unregister_alg(&alg);
}
