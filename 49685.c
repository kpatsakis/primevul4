static void __exit seed_fini(void)
{
	crypto_unregister_alg(&seed_alg);
}
