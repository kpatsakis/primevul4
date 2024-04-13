static int __init sha1_generic_mod_init(void)
{
	return crypto_register_shash(&alg);
}
