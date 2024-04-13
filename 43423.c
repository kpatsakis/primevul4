card_id_store_attr(struct device *dev, struct device_attribute *attr,
		   const char *buf, size_t count)
{
	struct snd_card *card = container_of(dev, struct snd_card, card_dev);
	char buf1[sizeof(card->id)];
	size_t copy = count > sizeof(card->id) - 1 ?
					sizeof(card->id) - 1 : count;
	size_t idx;
	int c;

	for (idx = 0; idx < copy; idx++) {
		c = buf[idx];
		if (!isalnum(c) && c != '_' && c != '-')
			return -EINVAL;
	}
	memcpy(buf1, buf, copy);
	buf1[copy] = '\0';
	mutex_lock(&snd_card_mutex);
	if (!card_id_ok(NULL, buf1)) {
		mutex_unlock(&snd_card_mutex);
		return -EEXIST;
	}
	strcpy(card->id, buf1);
	snd_info_card_id_change(card);
	mutex_unlock(&snd_card_mutex);

	return count;
}
