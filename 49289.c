static void __exit cast5_exit(void)
{
	crypto_unregister_algs(cast5_algs, ARRAY_SIZE(cast5_algs));
}
