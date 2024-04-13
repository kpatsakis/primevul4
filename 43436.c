int snd_card_free_when_closed(struct snd_card *card)
{
	int ret = snd_card_disconnect(card);
	if (ret)
		return ret;
	put_device(&card->card_dev);
	return 0;
}
