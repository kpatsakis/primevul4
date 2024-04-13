static void __exit blowfish_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
