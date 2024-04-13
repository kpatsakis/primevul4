static void __exit crypto_cmac_module_exit(void)
{
	crypto_unregister_template(&crypto_cmac_tmpl);
}
