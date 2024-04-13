static void snd_msnd_unload(struct snd_card *card)
{
	struct snd_msnd *chip = card->private_data;

	iounmap(chip->mappedbase);
	release_mem_region(chip->base, BUFFSIZE);
	release_region(chip->io, DSP_NUMIO);
	free_irq(chip->irq, chip);
	snd_card_free(card);
}
