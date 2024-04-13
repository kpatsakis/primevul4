static void __exit crypto_ctr_module_exit(void)
{
	crypto_unregister_template(&crypto_rfc3686_tmpl);
	crypto_unregister_template(&crypto_ctr_tmpl);
}
