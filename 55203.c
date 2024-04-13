int snd_seq_port_disconnect(struct snd_seq_client *connector,
			    struct snd_seq_client *src_client,
			    struct snd_seq_client_port *src_port,
			    struct snd_seq_client *dest_client,
			    struct snd_seq_client_port *dest_port,
			    struct snd_seq_port_subscribe *info)
{
	struct snd_seq_port_subs_info *src = &src_port->c_src;
	struct snd_seq_subscribers *subs;
	int err = -ENOENT;

	down_write(&src->list_mutex);
	/* look for the connection */
	list_for_each_entry(subs, &src->list_head, src_list) {
		if (match_subs_info(info, &subs->info)) {
			atomic_dec(&subs->ref_count); /* mark as not ready */
			err = 0;
			break;
		}
	}
	up_write(&src->list_mutex);
	if (err < 0)
		return err;

	delete_and_unsubscribe_port(src_client, src_port, subs, true,
				    connector->number != src_client->number);
	delete_and_unsubscribe_port(dest_client, dest_port, subs, false,
				    connector->number != dest_client->number);
	kfree(subs);
	return 0;
}
