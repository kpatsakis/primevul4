static int snd_card_do_free(struct snd_card *card)
{
#if IS_ENABLED(CONFIG_SND_MIXER_OSS)
	if (snd_mixer_oss_notify_callback)
		snd_mixer_oss_notify_callback(card, SND_MIXER_OSS_NOTIFY_FREE);
#endif
	snd_device_free_all(card);
	if (card->private_free)
		card->private_free(card);
	snd_info_free_entry(card->proc_id);
	if (snd_info_card_free(card) < 0) {
		dev_warn(card->dev, "unable to free card info\n");
		/* Not fatal error */
	}
	if (card->release_completion)
		complete(card->release_completion);
	kfree(card);
	return 0;
}
