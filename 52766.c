static int snd_seq_ioctl_get_client_info(struct snd_seq_client *client,
					 void __user *arg)
{
	struct snd_seq_client *cptr;
	struct snd_seq_client_info client_info;

	if (copy_from_user(&client_info, arg, sizeof(client_info)))
		return -EFAULT;

	/* requested client number */
	cptr = snd_seq_client_use_ptr(client_info.client);
	if (cptr == NULL)
		return -ENOENT;		/* don't change !!! */

	get_client_info(cptr, &client_info);
	snd_seq_client_unlock(cptr);

	if (copy_to_user(arg, &client_info, sizeof(client_info)))
		return -EFAULT;
	return 0;
}
