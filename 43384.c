static int snd_ctl_elem_info_user(struct snd_ctl_file *ctl,
				  struct snd_ctl_elem_info __user *_info)
{
	struct snd_ctl_elem_info info;
	int result;

	if (copy_from_user(&info, _info, sizeof(info)))
		return -EFAULT;
	snd_power_lock(ctl->card);
	result = snd_power_wait(ctl->card, SNDRV_CTL_POWER_D0);
	if (result >= 0)
		result = snd_ctl_elem_info(ctl, &info);
	snd_power_unlock(ctl->card);
	if (result >= 0)
		if (copy_to_user(_info, &info, sizeof(info)))
			return -EFAULT;
	return result;
}
