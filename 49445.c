static void __exit twofish_exit(void)
{
	crypto_unregister_algs(twofish_algs, ARRAY_SIZE(twofish_algs));
}
