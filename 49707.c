static int __init sha256_generic_mod_init(void)
{
	return crypto_register_shashes(sha256_algs, ARRAY_SIZE(sha256_algs));
}
