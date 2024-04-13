static int __init crypto_xcbc_module_init(void)
{
	return crypto_register_template(&crypto_xcbc_tmpl);
}
