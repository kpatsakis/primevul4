static int __init nx842_mod_init(void)
{
	del_timer(&failover_timer);
	return crypto_register_alg(&alg);
}
