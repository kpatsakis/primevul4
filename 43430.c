static void release_card_device(struct device *dev)
{
	snd_card_do_free(dev_to_snd_card(dev));
}
