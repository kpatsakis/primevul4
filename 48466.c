static void __exit crypto_gcm_module_exit(void)
{
	kfree(gcm_zeroes);
	crypto_unregister_template(&crypto_rfc4543_tmpl);
	crypto_unregister_template(&crypto_rfc4106_tmpl);
	crypto_unregister_template(&crypto_gcm_tmpl);
	crypto_unregister_template(&crypto_gcm_base_tmpl);
}
