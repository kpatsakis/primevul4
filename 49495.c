static int __init anubis_mod_init(void)
{
	int ret = 0;

	ret = crypto_register_alg(&anubis_alg);
	return ret;
}
