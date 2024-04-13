static void __exit snd_msnd_exit(void)
{
#ifdef CONFIG_PNP
	if (pnp_registered)
		pnp_unregister_card_driver(&msnd_pnpc_driver);
	if (isa_registered)
#endif
		isa_unregister_driver(&snd_msnd_driver);
}
