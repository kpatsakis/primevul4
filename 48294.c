static void __exit crypto_authenc_esn_module_exit(void)
{
	crypto_unregister_template(&crypto_authenc_esn_tmpl);
}
