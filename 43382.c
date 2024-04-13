static int snd_ctl_elem_add_user(struct snd_ctl_file *file,
				 struct snd_ctl_elem_info __user *_info, int replace)
{
	struct snd_ctl_elem_info info;
	if (copy_from_user(&info, _info, sizeof(info)))
		return -EFAULT;
	return snd_ctl_elem_add(file, &info, replace);
}
