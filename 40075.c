ax25_cb *ax25_find_cb(ax25_address *src_addr, ax25_address *dest_addr,
	ax25_digi *digi, struct net_device *dev)
{
	ax25_cb *s;

	spin_lock_bh(&ax25_list_lock);
	ax25_for_each(s, &ax25_list) {
		if (s->sk && s->sk->sk_type != SOCK_SEQPACKET)
			continue;
		if (s->ax25_dev == NULL)
			continue;
		if (ax25cmp(&s->source_addr, src_addr) == 0 && ax25cmp(&s->dest_addr, dest_addr) == 0 && s->ax25_dev->dev == dev) {
			if (digi != NULL && digi->ndigi != 0) {
				if (s->digipeat == NULL)
					continue;
				if (ax25digicmp(s->digipeat, digi) != 0)
					continue;
			} else {
				if (s->digipeat != NULL && s->digipeat->ndigi != 0)
					continue;
			}
			ax25_cb_hold(s);
			spin_unlock_bh(&ax25_list_lock);

			return s;
		}
	}
	spin_unlock_bh(&ax25_list_lock);

	return NULL;
}
