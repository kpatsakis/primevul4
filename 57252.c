static int __init snd_msnd_init(void)
{
	int err;

	err = isa_register_driver(&snd_msnd_driver, SNDRV_CARDS);
#ifdef CONFIG_PNP
	if (!err)
		isa_registered = 1;

	err = pnp_register_card_driver(&msnd_pnpc_driver);
	if (!err)
		pnp_registered = 1;

	if (isa_registered)
		err = 0;
#endif
	return err;
}
