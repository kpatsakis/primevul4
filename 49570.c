static int __init crct10dif_mod_init(void)
{
	int ret;

	ret = crypto_register_shash(&alg);
	return ret;
}
