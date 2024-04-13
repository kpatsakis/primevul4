static void __exit eseqiv_module_exit(void)
{
	crypto_unregister_template(&eseqiv_tmpl);
}
