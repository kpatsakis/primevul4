static void snd_card_module_info_read(struct snd_info_entry *entry,
				      struct snd_info_buffer *buffer)
{
	int idx;
	struct snd_card *card;

	for (idx = 0; idx < SNDRV_CARDS; idx++) {
		mutex_lock(&snd_card_mutex);
		if ((card = snd_cards[idx]) != NULL)
			snd_iprintf(buffer, "%2i %s\n",
				    idx, card->module->name);
		mutex_unlock(&snd_card_mutex);
	}
}
