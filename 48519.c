static int __init crypto_module_init(void)
{
	return crypto_register_template(&crypto_tmpl);
}
