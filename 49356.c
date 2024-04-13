static void __exit des3_ede_x86_fini(void)
{
	crypto_unregister_algs(des3_ede_algs, ARRAY_SIZE(des3_ede_algs));
}
