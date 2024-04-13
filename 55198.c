int snd_seq_delete_port(struct snd_seq_client *client, int port)
{
	unsigned long flags;
	struct snd_seq_client_port *found = NULL, *p;

	mutex_lock(&client->ports_mutex);
	write_lock_irqsave(&client->ports_lock, flags);
	list_for_each_entry(p, &client->ports_list_head, list) {
		if (p->addr.port == port) {
			/* ok found.  delete from the list at first */
			list_del(&p->list);
			client->num_ports--;
			found = p;
			break;
		}
	}
	write_unlock_irqrestore(&client->ports_lock, flags);
	mutex_unlock(&client->ports_mutex);
	if (found)
		return port_delete(client, found);
	else
		return -ENOENT;
}
