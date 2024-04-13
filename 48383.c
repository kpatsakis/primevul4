static void __exit cryptd_exit(void)
{
	cryptd_fini_queue(&queue);
	crypto_unregister_template(&cryptd_tmpl);
}
