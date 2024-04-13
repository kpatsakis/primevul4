static int __init crypto_cbc_module_init(void)
{
	return crypto_register_template(&crypto_cbc_tmpl);
}
