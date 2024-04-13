static void __exit salsa20_generic_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
