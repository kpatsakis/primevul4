static int __init sha1_powerpc_mod_init(void)
{
	return crypto_register_shash(&alg);
}
