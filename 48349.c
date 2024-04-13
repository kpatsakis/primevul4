static void chainiv_module_exit(void)
{
	crypto_unregister_template(&chainiv_tmpl);
}
