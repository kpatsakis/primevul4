static int __init lz4hc_mod_init(void)
{
	return crypto_register_alg(&alg_lz4hc);
}
