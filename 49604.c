static void __exit fcrypt_mod_fini(void)
{
	crypto_unregister_alg(&fcrypt_alg);
}
