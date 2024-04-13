static void __exit crypto_cts_module_exit(void)
{
	crypto_unregister_template(&crypto_cts_tmpl);
}
