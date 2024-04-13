static int snd_msnd_attach(struct snd_card *card)
{
	struct snd_msnd *chip = card->private_data;
	int err;
	static struct snd_device_ops ops = {
		.dev_free =      snd_msnd_dev_free,
		};

	err = request_irq(chip->irq, snd_msnd_interrupt, 0, card->shortname,
			  chip);
	if (err < 0) {
		printk(KERN_ERR LOGNAME ": Couldn't grab IRQ %d\n", chip->irq);
		return err;
	}
	if (request_region(chip->io, DSP_NUMIO, card->shortname) == NULL) {
		free_irq(chip->irq, chip);
		return -EBUSY;
	}

	if (!request_mem_region(chip->base, BUFFSIZE, card->shortname)) {
		printk(KERN_ERR LOGNAME
			": unable to grab memory region 0x%lx-0x%lx\n",
			chip->base, chip->base + BUFFSIZE - 1);
		release_region(chip->io, DSP_NUMIO);
		free_irq(chip->irq, chip);
		return -EBUSY;
	}
	chip->mappedbase = ioremap_nocache(chip->base, 0x8000);
	if (!chip->mappedbase) {
		printk(KERN_ERR LOGNAME
			": unable to map memory region 0x%lx-0x%lx\n",
			chip->base, chip->base + BUFFSIZE - 1);
		err = -EIO;
		goto err_release_region;
	}

	err = snd_msnd_dsp_full_reset(card);
	if (err < 0)
		goto err_release_region;

	/* Register device */
	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops);
	if (err < 0)
		goto err_release_region;

	err = snd_msnd_pcm(card, 0);
	if (err < 0) {
		printk(KERN_ERR LOGNAME ": error creating new PCM device\n");
		goto err_release_region;
	}

	err = snd_msndmix_new(card);
	if (err < 0) {
		printk(KERN_ERR LOGNAME ": error creating new Mixer device\n");
		goto err_release_region;
	}


	if (mpu_io[0] != SNDRV_AUTO_PORT) {
		struct snd_mpu401 *mpu;

		err = snd_mpu401_uart_new(card, 0, MPU401_HW_MPU401,
					  mpu_io[0],
					  MPU401_MODE_INPUT |
					  MPU401_MODE_OUTPUT,
					  mpu_irq[0],
					  &chip->rmidi);
		if (err < 0) {
			printk(KERN_ERR LOGNAME
				": error creating new Midi device\n");
			goto err_release_region;
		}
		mpu = chip->rmidi->private_data;

		mpu->open_input = snd_msnd_mpu401_open;
		mpu->close_input = snd_msnd_mpu401_close;
		mpu->private_data = chip;
	}

	disable_irq(chip->irq);
	snd_msnd_calibrate_adc(chip, chip->play_sample_rate);
	snd_msndmix_force_recsrc(chip, 0);

	err = snd_card_register(card);
	if (err < 0)
		goto err_release_region;

	return 0;

err_release_region:
	iounmap(chip->mappedbase);
	release_mem_region(chip->base, BUFFSIZE);
	release_region(chip->io, DSP_NUMIO);
	free_irq(chip->irq, chip);
	return err;
}
