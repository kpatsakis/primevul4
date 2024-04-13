static int snd_ctl_elem_write_user(struct snd_ctl_file *file,
				   struct snd_ctl_elem_value __user *_control)
{
	struct snd_ctl_elem_value *control;
	struct snd_card *card;
	int result;

	control = memdup_user(_control, sizeof(*control));
	if (IS_ERR(control))
		return PTR_ERR(control);

	card = file->card;
	snd_power_lock(card);
	result = snd_power_wait(card, SNDRV_CTL_POWER_D0);
	if (result >= 0)
		result = snd_ctl_elem_write(card, file, control);
	snd_power_unlock(card);
	if (result >= 0)
		if (copy_to_user(_control, control, sizeof(*control)))
			result = -EFAULT;
	kfree(control);
	return result;
}
