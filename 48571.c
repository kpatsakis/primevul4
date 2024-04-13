static int __init crypto_pcbc_module_init(void)
{
	return crypto_register_template(&crypto_pcbc_tmpl);
}
