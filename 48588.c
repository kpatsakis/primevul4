static void __exit pcrypt_exit(void)
{
	pcrypt_fini_padata(&pencrypt);
	pcrypt_fini_padata(&pdecrypt);

	kset_unregister(pcrypt_kset);
	crypto_unregister_template(&pcrypt_tmpl);
}
