static int snd_ctl_card_info(struct snd_card *card, struct snd_ctl_file * ctl,
			     unsigned int cmd, void __user *arg)
{
	struct snd_ctl_card_info *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (! info)
		return -ENOMEM;
	down_read(&snd_ioctl_rwsem);
	info->card = card->number;
	strlcpy(info->id, card->id, sizeof(info->id));
	strlcpy(info->driver, card->driver, sizeof(info->driver));
	strlcpy(info->name, card->shortname, sizeof(info->name));
	strlcpy(info->longname, card->longname, sizeof(info->longname));
	strlcpy(info->mixername, card->mixername, sizeof(info->mixername));
	strlcpy(info->components, card->components, sizeof(info->components));
	up_read(&snd_ioctl_rwsem);
	if (copy_to_user(arg, info, sizeof(struct snd_ctl_card_info))) {
		kfree(info);
		return -EFAULT;
	}
	kfree(info);
	return 0;
}
