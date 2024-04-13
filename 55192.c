static void delete_and_unsubscribe_port(struct snd_seq_client *client,
					struct snd_seq_client_port *port,
					struct snd_seq_subscribers *subs,
					bool is_src, bool ack)
{
	struct snd_seq_port_subs_info *grp;
	struct list_head *list;
	bool empty;

	grp = is_src ? &port->c_src : &port->c_dest;
	list = is_src ? &subs->src_list : &subs->dest_list;
	down_write(&grp->list_mutex);
	write_lock_irq(&grp->list_lock);
	empty = list_empty(list);
	if (!empty)
		list_del_init(list);
	grp->exclusive = 0;
	write_unlock_irq(&grp->list_lock);
	up_write(&grp->list_mutex);

	if (!empty)
		unsubscribe_port(client, port, grp, &subs->info, ack);
}
