static void snd_msnd_pnp_remove(struct pnp_card_link *pcard)
{
	snd_msnd_unload(pnp_get_card_drvdata(pcard));
	pnp_set_card_drvdata(pcard, NULL);
}
