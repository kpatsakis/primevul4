static int __init vmac_module_init(void)
{
	return crypto_register_template(&vmac_tmpl);
}
