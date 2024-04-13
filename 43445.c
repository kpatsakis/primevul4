void snd_card_set_id(struct snd_card *card, const char *nid)
{
	/* check if user specified own card->id */
	if (card->id[0] != '\0')
		return;
	mutex_lock(&snd_card_mutex);
	snd_card_set_id_no_lock(card, nid, nid);
	mutex_unlock(&snd_card_mutex);
}
