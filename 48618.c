static void __exit vmac_module_exit(void)
{
	crypto_unregister_template(&vmac_tmpl);
}
