static int snd_msnd_pnp_detect(struct pnp_card_link *pcard,
			       const struct pnp_card_device_id *pid)
{
	static int idx;
	struct pnp_dev *pnp_dev;
	struct pnp_dev *mpu_dev;
	struct snd_card *card;
	struct snd_msnd *chip;
	int ret;

	for ( ; idx < SNDRV_CARDS; idx++) {
		if (has_isapnp(idx))
			break;
	}
	if (idx >= SNDRV_CARDS)
		return -ENODEV;

	/*
	 * Check that we still have room for another sound card ...
	 */
	pnp_dev = pnp_request_card_device(pcard, pid->devs[0].id, NULL);
	if (!pnp_dev)
		return -ENODEV;

	mpu_dev = pnp_request_card_device(pcard, pid->devs[1].id, NULL);
	if (!mpu_dev)
		return -ENODEV;

	if (!pnp_is_active(pnp_dev) && pnp_activate_dev(pnp_dev) < 0) {
		printk(KERN_INFO "msnd_pinnacle: device is inactive\n");
		return -EBUSY;
	}

	if (!pnp_is_active(mpu_dev) && pnp_activate_dev(mpu_dev) < 0) {
		printk(KERN_INFO "msnd_pinnacle: MPU device is inactive\n");
		return -EBUSY;
	}

	/*
	 * Create a new ALSA sound card entry, in anticipation
	 * of detecting our hardware ...
	 */
	ret = snd_card_new(&pcard->card->dev,
			   index[idx], id[idx], THIS_MODULE,
			   sizeof(struct snd_msnd), &card);
	if (ret < 0)
		return ret;

	chip = card->private_data;
	chip->card = card;

	/*
	 * Read the correct parameters off the ISA PnP bus ...
	 */
	io[idx] = pnp_port_start(pnp_dev, 0);
	irq[idx] = pnp_irq(pnp_dev, 0);
	mem[idx] = pnp_mem_start(pnp_dev, 0);
	mpu_io[idx] = pnp_port_start(mpu_dev, 0);
	mpu_irq[idx] = pnp_irq(mpu_dev, 0);

	set_default_audio_parameters(chip);
#ifdef MSND_CLASSIC
	chip->type = msndClassic;
#else
	chip->type = msndPinnacle;
#endif
	chip->io = io[idx];
	chip->irq = irq[idx];
	chip->base = mem[idx];

	chip->calibrate_signal = calibrate_signal ? 1 : 0;
	chip->recsrc = 0;
	chip->dspq_data_buff = DSPQ_DATA_BUFF;
	chip->dspq_buff_size = DSPQ_BUFF_SIZE;
	if (write_ndelay[idx])
		clear_bit(F_DISABLE_WRITE_NDELAY, &chip->flags);
	else
		set_bit(F_DISABLE_WRITE_NDELAY, &chip->flags);
#ifndef MSND_CLASSIC
	if (digital[idx])
		set_bit(F_HAVEDIGITAL, &chip->flags);
#endif
	spin_lock_init(&chip->lock);
	ret = snd_msnd_probe(card);
	if (ret < 0) {
		printk(KERN_ERR LOGNAME ": Probe failed\n");
		goto _release_card;
	}

	ret = snd_msnd_attach(card);
	if (ret < 0) {
		printk(KERN_ERR LOGNAME ": Attach failed\n");
		goto _release_card;
	}

	pnp_set_card_drvdata(pcard, card);
	++idx;
	return 0;

_release_card:
	snd_card_free(card);
	return ret;
}
