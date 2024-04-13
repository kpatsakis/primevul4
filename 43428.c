static inline int init_info_for_card(struct snd_card *card)
{
	int err;
	struct snd_info_entry *entry;

	if ((err = snd_info_card_register(card)) < 0) {
		dev_dbg(card->dev, "unable to create card info\n");
		return err;
	}
	if ((entry = snd_info_create_card_entry(card, "id", card->proc_root)) == NULL) {
		dev_dbg(card->dev, "unable to create card entry\n");
		return err;
	}
	entry->c.text.read = snd_card_id_read;
	if (snd_info_register(entry) < 0) {
		snd_info_free_entry(entry);
		entry = NULL;
	}
	card->proc_id = entry;
	return 0;
}
