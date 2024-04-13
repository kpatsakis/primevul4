static void __exit crypto_pcbc_module_exit(void)
{
	crypto_unregister_template(&crypto_pcbc_tmpl);
}
