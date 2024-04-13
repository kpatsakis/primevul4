static void __exit arc4_exit(void)
{
	crypto_unregister_algs(arc4_algs, ARRAY_SIZE(arc4_algs));
}
