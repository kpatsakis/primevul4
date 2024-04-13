int snd_card_register(struct snd_card *card)
{
	int err;

	if (snd_BUG_ON(!card))
		return -EINVAL;

	if (!card->registered) {
		err = device_add(&card->card_dev);
		if (err < 0)
			return err;
		card->registered = true;
	}

	if ((err = snd_device_register_all(card)) < 0)
		return err;
	mutex_lock(&snd_card_mutex);
	if (snd_cards[card->number]) {
		/* already registered */
		mutex_unlock(&snd_card_mutex);
		return 0;
	}
	if (*card->id) {
		/* make a unique id name from the given string */
		char tmpid[sizeof(card->id)];
		memcpy(tmpid, card->id, sizeof(card->id));
		snd_card_set_id_no_lock(card, tmpid, tmpid);
	} else {
		/* create an id from either shortname or longname */
		const char *src;
		src = *card->shortname ? card->shortname : card->longname;
		snd_card_set_id_no_lock(card, src,
					retrieve_id_from_card_name(src));
	}
	snd_cards[card->number] = card;
	mutex_unlock(&snd_card_mutex);
	init_info_for_card(card);
#if IS_ENABLED(CONFIG_SND_MIXER_OSS)
	if (snd_mixer_oss_notify_callback)
		snd_mixer_oss_notify_callback(card, SND_MIXER_OSS_NOTIFY_REGISTER);
#endif
	return 0;
}
