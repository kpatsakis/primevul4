static int __init sha1_mod_init(void)
{
	return crypto_register_shash(&alg);
}
