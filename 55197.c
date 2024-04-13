int snd_seq_delete_all_ports(struct snd_seq_client *client)
{
	unsigned long flags;
	struct list_head deleted_list;
	struct snd_seq_client_port *port, *tmp;
	
	/* move the port list to deleted_list, and
	 * clear the port list in the client data.
	 */
	mutex_lock(&client->ports_mutex);
	write_lock_irqsave(&client->ports_lock, flags);
	if (! list_empty(&client->ports_list_head)) {
		list_add(&deleted_list, &client->ports_list_head);
		list_del_init(&client->ports_list_head);
	} else {
		INIT_LIST_HEAD(&deleted_list);
	}
	client->num_ports = 0;
	write_unlock_irqrestore(&client->ports_lock, flags);

	/* remove each port in deleted_list */
	list_for_each_entry_safe(port, tmp, &deleted_list, list) {
		list_del(&port->list);
		snd_seq_system_client_ev_port_exit(port->addr.client, port->addr.port);
		port_delete(client, port);
	}
	mutex_unlock(&client->ports_mutex);
	return 0;
}
