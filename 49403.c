static void __exit serpent_sse2_exit(void)
{
	crypto_unregister_algs(serpent_algs, ARRAY_SIZE(serpent_algs));
}
