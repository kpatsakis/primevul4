static void __exit cast6_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
