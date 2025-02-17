static int snd_seq_ioctl_create_port(struct snd_seq_client *client,
				     void __user *arg)
{
	struct snd_seq_client_port *port;
	struct snd_seq_port_info info;
	struct snd_seq_port_callback *callback;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	/* it is not allowed to create the port for an another client */
	if (info.addr.client != client->number)
		return -EPERM;

	port = snd_seq_create_port(client, (info.flags & SNDRV_SEQ_PORT_FLG_GIVEN_PORT) ? info.addr.port : -1);
	if (port == NULL)
		return -ENOMEM;

	if (client->type == USER_CLIENT && info.kernel) {
		snd_seq_delete_port(client, port->addr.port);
		return -EINVAL;
	}
	if (client->type == KERNEL_CLIENT) {
		if ((callback = info.kernel) != NULL) {
			if (callback->owner)
				port->owner = callback->owner;
			port->private_data = callback->private_data;
			port->private_free = callback->private_free;
			port->event_input = callback->event_input;
			port->c_src.open = callback->subscribe;
			port->c_src.close = callback->unsubscribe;
			port->c_dest.open = callback->use;
			port->c_dest.close = callback->unuse;
		}
	}

	info.addr = port->addr;

	snd_seq_set_port_info(port, &info);
	snd_seq_system_client_ev_port_start(port->addr.client, port->addr.port);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}
