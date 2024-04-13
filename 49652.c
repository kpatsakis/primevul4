static void __exit michael_mic_exit(void)
{
	crypto_unregister_shash(&alg);
}
