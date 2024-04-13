static int __init sha512_neon_mod_init(void)
{
	if (!cpu_has_neon())
		return -ENODEV;

	return crypto_register_shashes(algs, ARRAY_SIZE(algs));
}
