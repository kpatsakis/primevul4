static int __init crypto_cmac_module_init(void)
{
	return crypto_register_template(&crypto_cmac_tmpl);
}
