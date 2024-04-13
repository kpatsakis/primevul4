static void __exit cast5_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
