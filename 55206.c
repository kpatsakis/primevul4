static int subscribe_port(struct snd_seq_client *client,
			  struct snd_seq_client_port *port,
			  struct snd_seq_port_subs_info *grp,
			  struct snd_seq_port_subscribe *info,
			  int send_ack)
{
	int err = 0;

	if (!try_module_get(port->owner))
		return -EFAULT;
	grp->count++;
	if (grp->open && grp->count == 1) {
		err = grp->open(port->private_data, info);
		if (err < 0) {
			module_put(port->owner);
			grp->count--;
		}
	}
	if (err >= 0 && send_ack && client->type == USER_CLIENT)
		snd_seq_client_notify_subscription(port->addr.client, port->addr.port,
						   info, SNDRV_SEQ_EVENT_PORT_SUBSCRIBED);

	return err;
}
