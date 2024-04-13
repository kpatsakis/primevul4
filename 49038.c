static int __init sha1_neon_mod_init(void)
{
	if (!cpu_has_neon())
		return -ENODEV;

	return crypto_register_shash(&alg);
}
