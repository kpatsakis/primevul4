static int snd_seq_ioctl_get_subscription(struct snd_seq_client *client,
					  void __user *arg)
{
	int result;
	struct snd_seq_client *sender = NULL;
	struct snd_seq_client_port *sport = NULL;
	struct snd_seq_port_subscribe subs;
	struct snd_seq_subscribers *p;

	if (copy_from_user(&subs, arg, sizeof(subs)))
		return -EFAULT;

	result = -EINVAL;
	if ((sender = snd_seq_client_use_ptr(subs.sender.client)) == NULL)
		goto __end;
	if ((sport = snd_seq_port_use_ptr(sender, subs.sender.port)) == NULL)
		goto __end;
	p = snd_seq_port_get_subscription(&sport->c_src, &subs.dest);
	if (p) {
		result = 0;
		subs = p->info;
	} else
		result = -ENOENT;

      __end:
      	if (sport)
		snd_seq_port_unlock(sport);
	if (sender)
		snd_seq_client_unlock(sender);
	if (result >= 0) {
		if (copy_to_user(arg, &subs, sizeof(subs)))
			return -EFAULT;
	}
	return result;
}
