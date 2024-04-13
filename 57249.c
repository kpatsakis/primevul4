static int snd_msnd_dsp_full_reset(struct snd_card *card)
{
	struct snd_msnd *chip = card->private_data;
	int rv;

	if (test_bit(F_RESETTING, &chip->flags) || ++chip->nresets > 10)
		return 0;

	set_bit(F_RESETTING, &chip->flags);
	snd_msnd_dsp_halt(chip, NULL);	/* Unconditionally halt */

	rv = snd_msnd_initialize(card);
	if (rv)
		printk(KERN_WARNING LOGNAME ": DSP reset failed\n");
	snd_msndmix_force_recsrc(chip, 0);
	clear_bit(F_RESETTING, &chip->flags);
	return rv;
}
