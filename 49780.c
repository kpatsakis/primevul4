static void __exit padlock_fini(void)
{
	struct cpuinfo_x86 *c = &cpu_data(0);

	if (c->x86_model >= 0x0f) {
		crypto_unregister_shash(&sha1_alg_nano);
		crypto_unregister_shash(&sha256_alg_nano);
	} else {
		crypto_unregister_shash(&sha1_alg);
		crypto_unregister_shash(&sha256_alg);
	}
}
