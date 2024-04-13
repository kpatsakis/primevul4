static void __exit seqiv_module_exit(void)
{
	crypto_unregister_template(&seqiv_tmpl);
}
