static void __exit crypto_ccm_module_exit(void)
{
	crypto_unregister_template(&crypto_rfc4309_tmpl);
	crypto_unregister_template(&crypto_ccm_tmpl);
	crypto_unregister_template(&crypto_ccm_base_tmpl);
}
