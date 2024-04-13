static void __exit sha256_s390_fini(void)
{
	crypto_unregister_shash(&sha224_alg);
	crypto_unregister_shash(&sha256_alg);
}
