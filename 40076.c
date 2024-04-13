struct sock *ax25_find_listener(ax25_address *addr, int digi,
	struct net_device *dev, int type)
{
	ax25_cb *s;

	spin_lock(&ax25_list_lock);
	ax25_for_each(s, &ax25_list) {
		if ((s->iamdigi && !digi) || (!s->iamdigi && digi))
			continue;
		if (s->sk && !ax25cmp(&s->source_addr, addr) &&
		    s->sk->sk_type == type && s->sk->sk_state == TCP_LISTEN) {
			/* If device is null we match any device */
			if (s->ax25_dev == NULL || s->ax25_dev->dev == dev) {
				sock_hold(s->sk);
				spin_unlock(&ax25_list_lock);
				return s->sk;
			}
		}
	}
	spin_unlock(&ax25_list_lock);

	return NULL;
}
