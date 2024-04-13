static int check_and_subscribe_port(struct snd_seq_client *client,
				    struct snd_seq_client_port *port,
				    struct snd_seq_subscribers *subs,
				    bool is_src, bool exclusive, bool ack)
{
	struct snd_seq_port_subs_info *grp;
	struct list_head *p;
	struct snd_seq_subscribers *s;
	int err;

	grp = is_src ? &port->c_src : &port->c_dest;
	err = -EBUSY;
	down_write(&grp->list_mutex);
	if (exclusive) {
		if (!list_empty(&grp->list_head))
			goto __error;
	} else {
		if (grp->exclusive)
			goto __error;
		/* check whether already exists */
		list_for_each(p, &grp->list_head) {
			s = get_subscriber(p, is_src);
			if (match_subs_info(&subs->info, &s->info))
				goto __error;
		}
	}

	err = subscribe_port(client, port, grp, &subs->info, ack);
	if (err < 0) {
		grp->exclusive = 0;
		goto __error;
	}

	/* add to list */
	write_lock_irq(&grp->list_lock);
	if (is_src)
		list_add_tail(&subs->src_list, &grp->list_head);
	else
		list_add_tail(&subs->dest_list, &grp->list_head);
	grp->exclusive = exclusive;
	atomic_inc(&subs->ref_count);
	write_unlock_irq(&grp->list_lock);
	err = 0;

 __error:
	up_write(&grp->list_mutex);
	return err;
}
