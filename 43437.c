static void snd_card_id_read(struct snd_info_entry *entry,
			     struct snd_info_buffer *buffer)
{
	snd_iprintf(buffer, "%s\n", entry->card->id);
}
