static void __exit fini(void)
{
	crypto_unregister_algs(camellia_algs, ARRAY_SIZE(camellia_algs));
}
