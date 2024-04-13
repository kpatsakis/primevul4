static int __init crypto_authenc_module_init(void)
{
	return crypto_register_template(&crypto_authenc_tmpl);
}
