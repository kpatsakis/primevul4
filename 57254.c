static int snd_msnd_initialize(struct snd_card *card)
{
	struct snd_msnd *chip = card->private_data;
	int err, timeout;

#ifdef MSND_CLASSIC
	outb(HPWAITSTATE_0, chip->io + HP_WAIT);
	outb(HPBITMODE_16, chip->io + HP_BITM);

	reset_proteus(chip);
#endif
	err = snd_msnd_init_sma(chip);
	if (err < 0) {
		printk(KERN_WARNING LOGNAME ": Cannot initialize SMA\n");
		return err;
	}

	err = snd_msnd_reset_dsp(chip->io, NULL);
	if (err < 0)
		return err;

	err = upload_dsp_code(card);
	if (err < 0) {
		printk(KERN_WARNING LOGNAME ": Cannot upload DSP code\n");
		return err;
	}

	timeout = 200;

	while (readw(chip->mappedbase)) {
		msleep(1);
		if (!timeout--) {
			snd_printd(KERN_ERR LOGNAME ": DSP reset timeout\n");
			return -EIO;
		}
	}

	snd_msndmix_setup(chip);
	return 0;
}
