static int __init eseqiv_module_init(void)
{
	return crypto_register_template(&eseqiv_tmpl);
}
