static void __exit lzo_mod_fini(void)
{
	crypto_unregister_alg(&alg);
}
