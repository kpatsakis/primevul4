static int __init crypto_cts_module_init(void)
{
	return crypto_register_template(&crypto_cts_tmpl);
}
