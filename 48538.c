static void __exit mcryptd_exit(void)
{
	mcryptd_fini_queue(&mqueue);
	crypto_unregister_template(&mcryptd_tmpl);
	free_percpu(mcryptd_flist);
}
