static void __exit fini(void)
{
	crypto_unregister_alg(&alg);
}
