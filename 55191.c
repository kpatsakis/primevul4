static void clear_subscriber_list(struct snd_seq_client *client,
				  struct snd_seq_client_port *port,
				  struct snd_seq_port_subs_info *grp,
				  int is_src)
{
	struct list_head *p, *n;

	list_for_each_safe(p, n, &grp->list_head) {
		struct snd_seq_subscribers *subs;
		struct snd_seq_client *c;
		struct snd_seq_client_port *aport;

		subs = get_subscriber(p, is_src);
		if (is_src)
			aport = get_client_port(&subs->info.dest, &c);
		else
			aport = get_client_port(&subs->info.sender, &c);
		delete_and_unsubscribe_port(client, port, subs, is_src, false);

		if (!aport) {
			/* looks like the connected port is being deleted.
			 * we decrease the counter, and when both ports are deleted
			 * remove the subscriber info
			 */
			if (atomic_dec_and_test(&subs->ref_count))
				kfree(subs);
			continue;
		}

		/* ok we got the connected port */
		delete_and_unsubscribe_port(c, aport, subs, !is_src, true);
		kfree(subs);
		snd_seq_port_unlock(aport);
		snd_seq_client_unlock(c);
	}
}
