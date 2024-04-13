static int snd_seq_ioctl_query_next_client(struct snd_seq_client *client,
					   void __user *arg)
{
	struct snd_seq_client *cptr = NULL;
	struct snd_seq_client_info info;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	/* search for next client */
	info.client++;
	if (info.client < 0)
		info.client = 0;
	for (; info.client < SNDRV_SEQ_MAX_CLIENTS; info.client++) {
		cptr = snd_seq_client_use_ptr(info.client);
		if (cptr)
			break; /* found */
	}
	if (cptr == NULL)
		return -ENOENT;

	get_client_info(cptr, &info);
	snd_seq_client_unlock(cptr);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}
