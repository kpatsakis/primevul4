static void __exit hmac_module_exit(void)
{
	crypto_unregister_template(&hmac_tmpl);
}
