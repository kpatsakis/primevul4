static int snd_ctl_dev_disconnect(struct snd_device *device)
{
	struct snd_card *card = device->device_data;
	struct snd_ctl_file *ctl;
	int err, cardnum;

	if (snd_BUG_ON(!card))
		return -ENXIO;
	cardnum = card->number;
	if (snd_BUG_ON(cardnum < 0 || cardnum >= SNDRV_CARDS))
		return -ENXIO;

	read_lock(&card->ctl_files_rwlock);
	list_for_each_entry(ctl, &card->ctl_files, list) {
		wake_up(&ctl->change_sleep);
		kill_fasync(&ctl->fasync, SIGIO, POLL_ERR);
	}
	read_unlock(&card->ctl_files_rwlock);

	if ((err = snd_unregister_device(SNDRV_DEVICE_TYPE_CONTROL,
					 card, -1)) < 0)
		return err;
	return 0;
}
