static int __init crypto_ecb_module_init(void)
{
	return crypto_register_template(&crypto_ecb_tmpl);
}
