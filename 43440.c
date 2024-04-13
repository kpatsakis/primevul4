static void snd_card_info_read(struct snd_info_entry *entry,
			       struct snd_info_buffer *buffer)
{
	int idx, count;
	struct snd_card *card;

	for (idx = count = 0; idx < SNDRV_CARDS; idx++) {
		mutex_lock(&snd_card_mutex);
		if ((card = snd_cards[idx]) != NULL) {
			count++;
			snd_iprintf(buffer, "%2i [%-15s]: %s - %s\n",
					idx,
					card->id,
					card->driver,
					card->shortname);
			snd_iprintf(buffer, "                      %s\n",
					card->longname);
		}
		mutex_unlock(&snd_card_mutex);
	}
	if (!count)
		snd_iprintf(buffer, "--- no soundcards ---\n");
}
