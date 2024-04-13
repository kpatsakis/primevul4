static int snd_seq_ioctl_get_port_info(struct snd_seq_client *client,
				       void __user *arg)
{
	struct snd_seq_client *cptr;
	struct snd_seq_client_port *port;
	struct snd_seq_port_info info;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;
	cptr = snd_seq_client_use_ptr(info.addr.client);
	if (cptr == NULL)
		return -ENXIO;

	port = snd_seq_port_use_ptr(cptr, info.addr.port);
	if (port == NULL) {
		snd_seq_client_unlock(cptr);
		return -ENOENT;			/* don't change */
	}

	/* get port info */
	snd_seq_get_port_info(port, &info);
	snd_seq_port_unlock(port);
	snd_seq_client_unlock(cptr);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}
