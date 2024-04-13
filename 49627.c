static void __exit lz4hc_mod_fini(void)
{
	crypto_unregister_alg(&alg_lz4hc);
}
