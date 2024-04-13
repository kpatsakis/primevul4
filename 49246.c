static void __exit camellia_aesni_fini(void)
{
	crypto_unregister_algs(cmll_algs, ARRAY_SIZE(cmll_algs));
}
