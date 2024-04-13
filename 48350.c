static int __init chainiv_module_init(void)
{
	return crypto_register_template(&chainiv_tmpl);
}
