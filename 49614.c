static void __exit krng_mod_fini(void)
{
	crypto_unregister_alg(&krng_alg);
	return;
}
